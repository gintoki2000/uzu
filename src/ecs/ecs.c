#include "ecs/ecs.h"
#include "ecs/common.h"
#include "ecs/entity_pool.h"
#include "ecs/pool.h"
#include "toolbox/toolbox.h"

#include <stdlib.h>
#include <string.h>

INLINE void construct(const EcsType* type, void* component)
{
  if (type->init_fn != NULL)
    type->init_fn(component);
  else
    SDL_memset(component, 0, type->size);
}

INLINE void destruct(const EcsType* type, void* component)
{
  if (type->fini_fn != NULL)
    type->fini_fn(component);
}

INLINE void copy(const EcsType* type, void* dst, const void* src)
{
  if (type->cpy_fn != NULL)
    type->cpy_fn(dst, src);
  else
    SDL_memcpy(dst, src, type->size);
}

Ecs* ecs_new(const EcsType* types, ecs_size_t cnt)
{
  return ecs_init(SDL_malloc(sizeof(Ecs)), types, cnt);
}

void ecs_del(Ecs* ecs)
{
  if (ecs)
  {
    ecs_fini(ecs);
    free(ecs);
  }
}

Ecs* ecs_init(Ecs* self, const EcsType* types, ecs_size_t cnt)
{
  self->type_cnt = cnt;
  self->types    = SDL_malloc(cnt * sizeof(EcsType));
  self->pools    = SDL_calloc(cnt, sizeof(void*));
  SDL_memcpy(self->types, types, cnt * sizeof(EcsType));
  for (int i = 0; i < cnt; ++i)
  {
    self->pools[i] = ecs_pool_new(types[i].size);
  }
  ecs_entity_pool_init(&self->entity_pool);
  self->on_add = dispatcher_new(cnt);
  self->on_rmv = dispatcher_new(cnt);
  return self;
}

static void cb_clear(void* udata, Ecs* ecs, ecs_entity_t entity)
{
  (void)udata;
  ecs_destroy(ecs, entity);
}

void ecs_fini(Ecs* self)
{
  ecs_each(self, NULL, cb_clear);
  for (int i = 0; i < self->type_cnt; ++i)
    ecs_pool_del(self->pools[i]);
  free(self->pools);
  free(self->types);
  ecs_entity_pool_fini(&self->entity_pool);
  dispatcher_destroy(self->on_add);
  dispatcher_destroy(self->on_rmv);
}

ecs_entity_t ecs_create(Ecs* self)
{
  return ecs_entity_pool_alloc_ent(&self->entity_pool);
}

void ecs_destroy(Ecs* self, ecs_entity_t entity)
{
  ecs_rmv_all(self, entity);
  ecs_entity_pool_free_ent(&self->entity_pool, entity);
}

void* ecs_add(Ecs* self, ecs_entity_t entity, ecs_size_t type_id)
{
  void* component;

  ASSERT(type_id < self->type_cnt && "invalid type");
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");

  component = ecs_pool_add(self->pools[type_id], entity);
  construct(&self->types[type_id], component);
  dispatcher_emit(self->on_add,
                  type_id,
                  &(EcsComponentEvent){ .entity = entity, .component = component });
  return component;
}

void ecs_rmv(Ecs* self, ecs_entity_t entity, ecs_size_t type_id)
{
  ASSERT(type_id < self->type_cnt && "invalid type");
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");

  void* component;

  if ((component = ecs_pool_get(self->pools[type_id], entity)) != NULL)
  {
    dispatcher_emit(self->on_rmv,
                    type_id,
                    &(EcsComponentEvent){ .entity = entity, .component = component });
    destruct(&self->types[type_id], component);
    ecs_pool_rmv(self->pools[type_id], entity);
  }
}

void ecs_rmv_all(Ecs* self, ecs_entity_t entity)
{
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");
  void*          component;
  const EcsType* types;
  EcsPool**      pools;

  types = self->types;
  pools = self->pools;
  EcsComponentEvent event;
  for (int i = 0; i < self->type_cnt; ++i)
  {
    if ((component = ecs_pool_get(pools[i], entity)) != NULL)
    {
      event.component = component;
      event.entity    = entity;
      dispatcher_emit(self->on_rmv, i, &event);
      destruct(&types[i], component);
      ecs_pool_rmv(pools[i], entity);
    }
  }
}

void* ecs_get(Ecs* self, ecs_entity_t entity, ecs_size_t type)
{
  ASSERT(type < self->type_cnt && "invalid type");
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");
  return ecs_pool_get(self->pools[type], entity);
}

void ecs_each(Ecs* self, void* user_data, ecs_each_fn_t each_fn)
{
  ecs_size_t    size;
  ecs_entity_t* entities;

  size     = self->entity_pool.size;
  entities = self->entity_pool.entities;
  if (self->entity_pool.destroyed_index == ECS_NULL_IDX)
  {
    for (int i = size - 1; i >= 0; --i)
      each_fn(user_data, self, entities[i]);
  }
  else
  {
    for (int i = size - 1; i >= 0; --i)
      if (ECS_ENT_IDX(entities[i]) == i)
        each_fn(user_data, self, entities[i]);
  }
}

void ecs_each_w_filter(Ecs*             self,
                       const EcsFilter* filter,
                       void*            user_data,
                       ecs_each_ex_fn_t each_fn)
{
  int      min_cnt;
  EcsPool* min_pool;

  ecs_entity_t* entities;
  ecs_size_t    entity_cnt;
  EcsPool**     pools;
  ecs_size_t    pool_cnt;
  void*         components[filter->rcnt];
  EcsPool*      tp;
  ecs_entity_t  te;
  SDL_bool      match;

  pool_cnt = self->type_cnt;
  pools    = self->pools;

  /**
   * tìm ra pool chứa ít phần tử nhất trong danh sách type
   * việc này giúp chúng giảm tối đa việc duyệt và so khớp nhất có thể
   */
  min_pool = pools[filter->required[0]];
  min_cnt  = ecs_pool_cnt(min_pool);

  for (int ti = 1; ti < filter->rcnt; ++ti)
  {
    tp = pools[filter->required[ti]];
    if (ecs_pool_cnt(tp) < min_cnt)
    {
      min_cnt  = ecs_pool_cnt(tp);
      min_pool = tp;
    }
  }

  /**
   * sau khi có được pool có ít phần từ nhất duyệt qua nó và kiểm tra
   * từng entity trong pool có khớp với tập component đầu vào hay không
   * nếu có thì lấy component tương ứng và đưa vào mảng tạm và gọi callback
   */
  entities   = min_pool->dense.entities;
  entity_cnt = min_pool->dense.cnt;

  for (int ei = 0; ei < entity_cnt; ++ei)
  {

    te    = entities[ei];
    match = SDL_TRUE;
    for (int ti = 0; ti < filter->rcnt; ++ti)
    {
      tp = pools[filter->required[ti]];
      if (tp != min_pool && !ecs_pool_contains(tp, te))
      {
        match = SDL_FALSE;
        break;
      }
    }
    if (match)
      for (int ti = 0; ti < filter->ecnt; ++ti)
      {
        if (ecs_pool_contains(tp, te))
        {
          match = SDL_FALSE;
          break;
        }
      }
    if (match)
    {
      for (int ti = 0; ti < filter->rcnt; ++ti)
      {
        components[ti] = ecs_pool_get(pools[filter->required[ti]], te);
      }
      each_fn(user_data, self, te, components);
    }
  }
}

void ecs_raw(Ecs*           self,
             ecs_size_t     type,
             ecs_entity_t** entities_ptr,
             void**         components_ptr,
             ecs_size_t*    cnt_ptr)
{
  ASSERT(type < self->type_cnt && "invalid type");
  if (entities_ptr)
    *entities_ptr = self->pools[type]->dense.entities;
  if (components_ptr)
    *components_ptr = self->pools[type]->dense.components;
  if (cnt_ptr)
    *cnt_ptr = self->pools[type]->dense.cnt;
}

SDL_bool ecs_has(Ecs* self, ecs_entity_t entity, ecs_size_t type_id)
{
  ASSERT(type_id < self->type_cnt && "invalid type");
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");
  return ecs_pool_contains(self->pools[type_id], entity);
}

void ecs_clear(Ecs* self)
{
  ecs_each(self, NULL, cb_clear);
}

void* ecs_set(Ecs* self, ecs_entity_t entity, ecs_size_t type_id, const void* data)
{
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");
  ASSERT(type_id < self->type_cnt && "invalid component type id");
  void* raw = ecs_pool_get(self->pools[type_id], entity);

  if (raw != NULL)
  {
    copy(&self->types[type_id], raw, data);
  }
  else
  {
    raw = ecs_pool_add(self->pools[type_id], entity);
    copy(&self->types[type_id], raw, data);
    dispatcher_emit(self->on_add,
                    type_id,
                    &(EcsComponentEvent){ .entity = entity, .component = raw });
  }
  return raw;
}

void ecs_on_add(Ecs* ecs, ecs_size_t type_id, funcptr_t fn, pointer_t arg)
{
  dispatcher_connect(ecs->on_add, type_id, arg, fn);
}

void ecs_on_rmv(Ecs* ecs, ecs_size_t type_id, funcptr_t fn, pointer_t arg)
{
  dispatcher_connect(ecs->on_rmv, type_id, arg, fn);
}
