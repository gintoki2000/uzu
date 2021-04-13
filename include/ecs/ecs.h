#ifndef WORLD_H
#define WORLD_H
#include "ecs/common.h"
#include "ecs/pool.h"
#include "toolbox/toolbox.h"

enum
{
  ECS_EVT_RMV_COMP,
  ECS_EVT_ADD_COMP,
  ECS_NUM_EVENTS,
};

typedef struct Ecs
{
  ecs_size_t    type_cnt;
  EcsType*      types;
  EcsPool**     pools;
  Emitter*      emitter[ECS_NUM_EVENTS];
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_size_t    size;
  ecs_size_t    destroyed_index;
} Ecs;

typedef struct EcsFilter
{
  const ecs_size_t* required;
  ecs_size_t        rcnt;
  const ecs_size_t* exclude;
  ecs_size_t        ecnt;
} EcsFilter;

typedef struct EcsComponentEvent
{
  ecs_entity_t entity;
  void*        component;
} EcsComponentEvent;

Ecs* ecs_new(const EcsType* types, ecs_size_t cnt);
void ecs_del(Ecs* ecs);

Ecs* ecs_init(Ecs* self, const EcsType* types, ecs_size_t type_cnt);
void ecs_fini(Ecs* self);

ecs_entity_t ecs_create(Ecs* self);
ecs_entity_t ecs_clone(Ecs* self, ecs_entity_t prototype);

void  ecs_destroy(Ecs* self, ecs_entity_t entity);
void* ecs_add(Ecs* self, ecs_entity_t entity, ecs_size_t type_id);
void* ecs_set(Ecs* self, ecs_entity_t entity, ecs_size_t type_id, const void* data);
void  ecs_rmv(Ecs* self, ecs_entity_t entity, ecs_size_t type_id);
void* ecs_get(Ecs* self, ecs_entity_t entity, ecs_size_t type_id);
BOOL  ecs_has(Ecs* self, ecs_entity_t entity, ecs_size_t type_id);
void  ecs_rmv_all(Ecs* self, ecs_entity_t entity);
void  ecs_each(Ecs* self, Callback callback);
void  ecs_each_ex(Ecs* self, const EcsFilter* filter, Callback callback);
void  ecs_clear(Ecs* self);
void  ecs_fill(Ecs* self, ecs_size_t entity, const ecs_size_t* types, ecs_size_t cnt, void** arr);
ecs_entity_t ecs_cpy(Ecs* dst_registry, Ecs* src_registry, ecs_entity_t src_entity);

void ecs_raw(Ecs*           self,
             ecs_size_t     type,
             ecs_entity_t** entities_ptr,
             void**         components_ptr,
             ecs_size_t*    cnt_ptr);

INLINE BOOL ecs_is_valid(Ecs* self, ecs_entity_t entity)
{
  ecs_size_t idx = ECS_ENT_IDX(entity);
  return (idx < self->size) && (self->entities[idx] == entity);
}

void ecs_connect(Ecs* self, int event, ecs_size_t type_id, Callback cb);
void ecs_disconnect(Ecs* self, int event, ecs_size_t type_id, void(*fn)());
#endif // WORLD_H
