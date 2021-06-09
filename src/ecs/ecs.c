#include "ecs/ecs.h"
#include "ecs/common.h"
#include "ecs/pool.h"
#include "toolbox/toolbox.h"

#include <stdlib.h>
#include <string.h>

#define ECS_DEFAULT_SIZE 16

#define ASSERT_VALID_ENTITY(registry, entity)                                                      \
  ASSERT(ecs_is_valid(registry, entity) && "invalid entity")
#define ASSERT_VALID_TYPE_ID(registry, type_id)                                                    \
  ASSERT((type_id) < registry->typeCount && "invalid type id")
#define ASSERT_VALID_EVENT_ID(event_id)                                                            \
  ASSERT((event_id >= 0 && event_id < ECS_NUM_EVENTS) && "invalid event id")

struct ecs_Registry
{
  ecs_CompDesc* types;
  ecs_Pool**     pools;
  Signal*       onConstruct;
  Signal*       onDestruct;
  ecs_entity_t* entities;
  ecs_size_t    count;
  ecs_size_t    size;
  ecs_size_t    destroyedIndex;
  ecs_size_t    typeCount;
};

INLINE void construct_component(const ecs_CompDesc* decs, void* component)
{
  if (decs->initFunc != NULL)
    decs->initFunc(component);
  else
    SDL_memset(component, 0, decs->size);
}

INLINE void destruct_component(const ecs_CompDesc* type, void* component)
{
  if (type->finiFunc != NULL)
    type->finiFunc(component);
}

INLINE void copy_component(const ecs_CompDesc* type, void* dst, const void* src)
{
  if (type->cpyFunc != NULL)
    type->cpyFunc(dst, src);
  else
    SDL_memcpy(dst, src, type->size);
}

ecs_Registry* ecs_registry_create(const ecs_CompDesc* types, ecs_size_t cnt)
{
  return ecs_registry_init(SDL_malloc(sizeof(ecs_Registry)), types, cnt);
}

void ecs_registry_free(ecs_Registry* ecs)
{
  if (ecs)
  {
    ecs_fini(ecs);
    SDL_free(ecs);
  }
}

ecs_Registry* ecs_registry_init(ecs_Registry* self, const ecs_CompDesc* types, ecs_size_t cnt)
{
  self->typeCount = cnt;
  self->types    = SDL_malloc(cnt * sizeof(ecs_CompDesc));
  self->pools    = SDL_calloc(cnt, sizeof(void*));
  SDL_memcpy(self->types, types, cnt * sizeof(ecs_CompDesc));
  for (int i = 0; i < cnt; ++i)
  {
    self->pools[i] = ecs_pool_create(types[i].size);
  }
  self->count             = 0;
  self->size            = ECS_DEFAULT_SIZE;
  self->destroyedIndex = 0;
  self->entities        = SDL_malloc(ECS_DEFAULT_SIZE * sizeof(ecs_entity_t));

  for (int i = 0; i < ECS_DEFAULT_SIZE - 1; ++i)
    self->entities[i] = ECS_ENT(i + 1, 0);

  self->entities[ECS_DEFAULT_SIZE - 1] = ECS_ENT(ECS_NULL_IDX, 0);
  self->onConstruct                   = SDL_calloc(cnt, sizeof(Signal));
  self->onDestruct                    = SDL_calloc(cnt, sizeof(Signal));
  for (int i = 0; i < cnt; ++i)
  {
    signal_init(&self->onConstruct[i]);
    signal_init(&self->onDestruct[i]);
  }
  return self;
}

static void clear_callback(void* udata, ecs_entity_t entity)
{
  ecs_destroy((ecs_Registry*)udata, entity);
}

void ecs_fini(ecs_Registry* self)
{
  ecs_each(self, CALLBACK_1(self, clear_callback));
  for (int i = 0; i < self->typeCount; ++i)
    ecs_pool_free(self->pools[i]);
  SDL_free(self->pools);
  SDL_free(self->types);
  SDL_free(self->entities);

  for (int i = 0; i < self->typeCount; ++i)
  {
    signal_fini(&self->onConstruct[i]);
    signal_fini(&self->onDestruct[i]);
  }
  SDL_free(self->onConstruct);
  SDL_free(self->onDestruct);
}

ecs_entity_t ecs_create(ecs_Registry* self)
{
  ecs_size_t ver;
  ecs_size_t idx;

  if (self->destroyedIndex == ECS_NULL_IDX)
  {
    self->size *= 2;
    self->entities = SDL_realloc(self->entities, self->size * sizeof(ecs_entity_t));
    for (int i = self->count; i < self->size - 1; ++i)
    {
      self->entities[i] = ECS_ENT(i + 1, 0);
    }
    self->entities[self->size - 1] = ECS_ENT(ECS_NULL_IDX, 0);
    self->destroyedIndex          = self->count;
  }

  idx                   = self->destroyedIndex;
  ver                   = ECS_ENT_VER(self->entities[idx]);
  self->destroyedIndex = ECS_ENT_IDX(self->entities[idx]);
  self->entities[idx]   = ECS_ENT(idx, ver);
  self->count++;

  return ECS_ENT(idx, ver);
}

INLINE void recycle(ecs_Registry* self, ecs_entity_t entity)
{
  ecs_size_t ent_ver;
  ecs_size_t ent_idx;

  ent_ver = ECS_ENT_VER(entity);
  ent_idx = ECS_ENT_IDX(entity);

  self->entities[ent_idx] = ECS_ENT(self->destroyedIndex, ent_ver + 1);
  self->destroyedIndex   = ent_idx;
  self->count--;
}

void ecs_destroy(ecs_Registry* self, ecs_entity_t entity)
{
  ecs_rmv_all(self, entity);
  recycle(self, entity);
}

void* ecs_add(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id)
{
  void* component;

  ASSERT_VALID_TYPE_ID(self, type_id);
  ASSERT_VALID_ENTITY(self, entity);

  component = ecs_pool_add(self->pools[type_id], entity);
  construct_component(&self->types[type_id], component);
  signal_emit(&self->onConstruct[type_id],
              &(ecs_ComponentAdded){
                  .entity    = entity,
                  .component = component,
              });
  return component;
}

void ecs_rmv(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id)
{
  ASSERT_VALID_TYPE_ID(self, type_id);
  ASSERT_VALID_ENTITY(self, entity);
  void* component;

  if ((component = ecs_pool_get(self->pools[type_id], entity)) != NULL)
  {
    signal_emit(&self->onDestruct[type_id],
                &(ecs_ComponentRemoved){
                    .entity    = entity,
                    .component = component,
                });
    destruct_component(&self->types[type_id], component);
    ecs_pool_rmv(self->pools[type_id], entity);
  }
}

void ecs_rmv_all(ecs_Registry* self, ecs_entity_t entity)
{
  ASSERT_VALID_ENTITY(self, entity);
  void*          component;
  const ecs_CompDesc* types;
  ecs_Pool**      pools;

  types = self->types;
  pools = self->pools;
  ecs_ComponentEvent event;
  for (int i = 0; i < self->typeCount; ++i)
  {
    if ((component = ecs_pool_get(pools[i], entity)) != NULL)
    {
      event.component = component;
      event.entity    = entity;
      signal_emit(&self->onDestruct[i], &event);
      destruct_component(&types[i], component);
      ecs_pool_rmv(pools[i], entity);
    }
  }
}

void* ecs_get(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type)
{
  ASSERT_VALID_TYPE_ID(self, type);
  ASSERT_VALID_ENTITY(self, entity);
  return ecs_pool_get(self->pools[type], entity);
}

void ecs_each(ecs_Registry* self, Callback callback)
{
  ecs_size_t    size;
  ecs_entity_t* entities;

  size                            = self->size;
  entities                        = self->entities;
  void (*fn)(void*, ecs_entity_t) = callback.func;
  if (self->destroyedIndex == ECS_NULL_IDX)
    for (int i = size - 1; i >= 0; --i)
      fn(callback.userData, entities[i]);
  else
    for (int i = size - 1; i >= 0; --i)
      if (ECS_ENT_IDX(entities[i]) == i)
        fn(callback.userData, entities[i]);
}

void ecs_raw(ecs_Registry*           self,
             ecs_size_t     type,
             ecs_entity_t** entities_ptr,
             void**         components_ptr,
             ecs_size_t*    cnt_ptr)
{
  ASSERT_VALID_TYPE_ID(self, type);
  if (entities_ptr)
    *entities_ptr = self->pools[type]->dense.entities;
  if (components_ptr)
    *components_ptr = self->pools[type]->dense.components;
  if (cnt_ptr)
    *cnt_ptr = self->pools[type]->dense.cnt;
}

SDL_bool ecs_has(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id)
{
  ASSERT_VALID_TYPE_ID(self, type_id);
  ASSERT_VALID_ENTITY(self, entity);
  return ecs_pool_contains(self->pools[type_id], entity);
}

void ecs_clear(ecs_Registry* self)
{
  ecs_each(self, CALLBACK_1(self, clear_callback));
}

void* ecs_set(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id, const void* data)
{
  ASSERT_VALID_TYPE_ID(self, type_id);
  ASSERT_VALID_ENTITY(self, entity);
  void* raw = ecs_pool_get(self->pools[type_id], entity);

  if (raw != NULL)
  {
    SDL_memcpy(raw, data, self->types[type_id].size);
  }
  else
  {
    raw = ecs_pool_add(self->pools[type_id], entity);
    SDL_memcpy(raw, data, self->types[type_id].size);
    signal_emit(&self->onConstruct[type_id],
                &(ecs_ComponentEvent){ .entity = entity, .component = raw });
  }
  return raw;
}

void ecs_fill(ecs_Registry* self, ecs_entity_t entity, const ecs_size_t* types, ecs_size_t cnt, void* arr[])
{
  ASSERT_VALID_ENTITY(self, entity);
  for (int i = 0; i < cnt; ++i)
  {
    ASSERT_VALID_TYPE_ID(self, types[i]);
    if ((arr[i] = ecs_pool_get(self->pools[types[i]], entity)) == NULL)
      break;
  }
}

ecs_entity_t ecs_cpy(ecs_Registry* dst_registry, ecs_Registry* src_registry, ecs_entity_t src_entity)
{
  ecs_entity_t clone_entity;
  void*        src_data;
  void*        cpy_data;
  ASSERT_VALID_ENTITY(src_registry, src_entity);

  clone_entity = ecs_create(dst_registry);
  for (int itype_id = 0; itype_id < dst_registry->typeCount; ++itype_id)
  {
    if ((src_data = ecs_pool_get(src_registry->pools[itype_id], src_entity)) != NULL)
    {
      cpy_data = ecs_pool_add(dst_registry->pools[itype_id], clone_entity);
      copy_component(&dst_registry->types[itype_id], cpy_data, src_data);
      signal_emit(&dst_registry->onConstruct[itype_id],
                  &(ecs_ComponentEvent){ .entity = clone_entity, .component = cpy_data });
    }
  }

  return clone_entity;
}

static ecs_size_t get_smallest_type_index(ecs_Registry* registry, const ecs_size_t* types, ecs_size_t count)
{
  ecs_size_t smallest_type_index = 0;
  for (int i = 1; i < count; ++i)
    if (ecs_pool_count(registry->pools[types[i]]) <
        ecs_pool_count(registry->pools[types[smallest_type_index]]))
      smallest_type_index = i;
  return smallest_type_index;
}

static BOOL has_any(ecs_Registry* self, ecs_entity_t entity, const ecs_size_t* types, ecs_size_t cnt)
{
  for (int i = 0; i < cnt; ++i)
    if (ecs_pool_contains(self->pools[types[i]], entity))
      return TRUE;
  return FALSE;
}

void ecs_each_ex(ecs_Registry* self, const ecs_Filter* filter, Callback callback)
{
  ecs_size_t smallest_type_index = get_smallest_type_index(self, filter->required, filter->rcnt);
  ecs_Pool**  pools               = self->pools;
  ecs_size_t count               = ecs_pool_count(pools[filter->required[smallest_type_index]]);
  const ecs_entity_t* entities   = pools[filter->required[smallest_type_index]]->dense.entities;
  void (*fn)(void*, ecs_entity_t, void**) = (void (*)(void*, ecs_entity_t, void**))callback.func;
  int   itype;
  void* components[filter->rcnt];
  BOOL  match;
  for (int iett = 0; iett < count; ++iett)
  {
    if (!has_any(self, entities[iett], filter->exclude, filter->ecnt))
    {
      match = TRUE;
      for (itype = 0; itype < filter->rcnt; ++itype)
      {
        if ((components[itype] = ecs_pool_get(pools[filter->required[itype]], entities[iett])) ==
            NULL)
        {
          match = FALSE;
          break;
        }
      }
      if (match)
        fn(callback.userData, entities[iett], components);
    }
  }
}

Signal* ecs_on_construct(ecs_Registry* self, ecs_size_t type)
{
  ASSERT_MSG(type < self->typeCount, "invalid component type");
  return &self->onConstruct[type];
}

Signal* ecs_on_destruct(ecs_Registry* self, ecs_size_t type)
{
  ASSERT_MSG(type < self->typeCount, "invalid component type");
  return &self->onDestruct[type];
}

SDL_bool ecs_is_valid(ecs_Registry *self, ecs_entity_t entity)
{
  ecs_size_t idx = ECS_ENT_IDX(entity);
  return (idx < self->size) && (self->entities[idx] == entity);
}
