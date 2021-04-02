#include "ecs/ecs.h"
#include "ecs/common.h"
#include "ecs/pool.h"
#include "toolbox/toolbox.h"

#include <stdlib.h>
#include <string.h>

#define ECS_DEFAULT_SIZE 16

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
    SDL_free(ecs);
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
  self->cnt             = 0;
  self->size            = ECS_DEFAULT_SIZE;
  self->destroyed_index = 0;
  self->entities        = SDL_malloc(ECS_DEFAULT_SIZE * sizeof(ecs_entity_t));

  for (int i = 0; i < ECS_DEFAULT_SIZE - 1; ++i)
    self->entities[i] = ECS_ENT(i + 1, 0);

  self->entities[ECS_DEFAULT_SIZE - 1] = ECS_ENT(ECS_NULL_IDX, 0);
  self->dispatcher[ECS_EVT_ADD_COMP]   = dispatcher_new(cnt);
  self->dispatcher[ECS_EVT_RMV_COMP]   = dispatcher_new(cnt);
  return self;
}

static void cb_clear(SDL_UNUSED void* udata, Ecs* ecs, ecs_entity_t entity)
{
  ecs_destroy(ecs, entity);
}

void ecs_fini(Ecs* self)
{
  ecs_each(self, NULL, cb_clear);
  for (int i = 0; i < self->type_cnt; ++i)
    ecs_pool_del(self->pools[i]);
  SDL_free(self->pools);
  SDL_free(self->types);
  SDL_free(self->entities);
  for (int i = 0; i < ECS_NUM_EVENTS; ++i)
  {
    dispatcher_destroy(self->dispatcher[i]);
  }
}

ecs_entity_t ecs_create(Ecs* self)
{
  ecs_size_t ver;
  ecs_size_t idx;

  if (self->destroyed_index == ECS_NULL_IDX)
  {
    self->size *= 2;
    self->entities = realloc(self->entities, self->size * sizeof(ecs_entity_t));
    for (int i = self->cnt; i < self->size - 1; ++i)
    {
      self->entities[i] = ECS_ENT(i + 1, 0);
    }
    self->entities[self->size - 1] = ECS_ENT(ECS_NULL_IDX, 0);
    self->destroyed_index          = self->cnt;
  }

  idx                   = self->destroyed_index;
  ver                   = ECS_ENT_VER(self->entities[idx]);
  self->destroyed_index = ECS_ENT_IDX(self->entities[idx]);
  self->entities[idx]   = ECS_ENT(idx, ver);
  self->cnt++;

  return ECS_ENT(idx, ver);
}

INLINE void recycle(Ecs* self, ecs_entity_t entity)
{
  ecs_size_t ent_ver;
  ecs_size_t ent_idx;

  ent_ver = ECS_ENT_VER(entity);
  ent_idx = ECS_ENT_IDX(entity);

  self->entities[ent_idx] = ECS_ENT(self->destroyed_index, ent_ver + 1);
  self->destroyed_index   = ent_idx;
  self->cnt--;
}

void ecs_destroy(Ecs* self, ecs_entity_t entity)
{
  ecs_rmv_all(self, entity);
  recycle(self, entity);
}

void* ecs_add(Ecs* self, ecs_entity_t entity, ecs_size_t type_id)
{
  void* component;

  ASSERT(type_id < self->type_cnt && "invalid type");
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");

  component = ecs_pool_add(self->pools[type_id], entity);
  construct(&self->types[type_id], component);
  dispatcher_emit(self->dispatcher[ECS_EVT_ADD_COMP],
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
    dispatcher_emit(self->dispatcher[ECS_EVT_RMV_COMP],
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
      dispatcher_emit(self->dispatcher[ECS_EVT_RMV_COMP], i, &event);
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

  size     = self->size;
  entities = self->entities;
  if (self->destroyed_index == ECS_NULL_IDX)
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
    dispatcher_emit(self->dispatcher[ECS_EVT_ADD_COMP],
                    type_id,
                    &(EcsComponentEvent){ .entity = entity, .component = raw });
  }
  return raw;
}

void ecs_connect(Ecs* self, int event, ecs_size_t type_id, Callback cb)
{
  ASSERT((event >= 0 && event < ECS_NUM_EVENTS) && "invalid event type");
  ASSERT(type_id < self->type_cnt && "invalid component type id");
  dispatcher_connect(self->dispatcher[event], type_id, cb.user_data, cb.func);
}

void ecs_disconnect(Ecs* self, int event, ecs_size_t type_id, pointer_t fn_or_arg)
{
  ASSERT((event >= 0 && event < ECS_NUM_EVENTS) && "invalid event type");
  ASSERT(type_id < self->type_cnt && "invalid component type id");
  dispatcher_disconnect(self->dispatcher[event], type_id, fn_or_arg);
}

void ecs_fill(Ecs* self, ecs_size_t entity, const ecs_size_t* types, ecs_size_t cnt, void** arr)
{
  ASSERT(ecs_is_valid(self, entity) && "invalid entity");
  for (int i = 0; i < cnt; ++i)
  {
    ASSERT(types[i] >= 0 && types[i] < self->type_cnt);
    arr[i] = ecs_pool_get(self->pools[types[i]], entity);
  }
}
