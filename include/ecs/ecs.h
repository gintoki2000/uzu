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



typedef struct ecs_Registry ecs_Registry;

typedef void(*ecs_EachFunc)(void* arg, ecs_entity_t entity);

typedef struct ecs_Filter
{
  const ecs_size_t* required;
  ecs_size_t        rcnt;
  const ecs_size_t* exclude;
  ecs_size_t        ecnt;
} ecs_Filter;

typedef struct ecs_ComponentEvent
{
  ecs_entity_t entity;
  void*        component;
} ecs_ComponentEvent, ecs_ComponentAdded, ecs_ComponentRemoved;

ecs_Registry* ecs_registry_create(const ecs_CompDesc* types, ecs_size_t cnt);
void ecs_registry_free(ecs_Registry* ecs);

ecs_Registry* ecs_registry_init(ecs_Registry* self, const ecs_CompDesc* types, ecs_size_t type_cnt);
void ecs_fini(ecs_Registry* self);

ecs_entity_t ecs_create(ecs_Registry* self);
ecs_entity_t ecs_clone(ecs_Registry* self, ecs_entity_t prototype);

void  ecs_destroy(ecs_Registry* self, ecs_entity_t entity);
void* ecs_add(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id);
void* ecs_set(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id, const void* data);
void  ecs_rmv(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id);
void* ecs_get(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id);
BOOL  ecs_has(ecs_Registry* self, ecs_entity_t entity, ecs_size_t type_id);
void  ecs_rmv_all(ecs_Registry* self, ecs_entity_t entity);
void  ecs_each(ecs_Registry* self, Callback callback);
void  ecs_each_ex(ecs_Registry* self, const ecs_Filter* filter, Callback callback);
void  ecs_clear(ecs_Registry* self);
void  ecs_fill(ecs_Registry* self, ecs_entity_t entity, const ecs_size_t* types, ecs_size_t cnt, void** arr);
ecs_entity_t ecs_cpy(ecs_Registry* dst_registry, ecs_Registry* src_registry, ecs_entity_t src_entity);

void ecs_raw(ecs_Registry*           self,
             ecs_size_t     type,
             ecs_entity_t** entities_ptr,
             void**         components_ptr,
             ecs_size_t*    cnt_ptr);

BOOL ecs_is_valid(ecs_Registry* self, ecs_entity_t entity);
Signal* ecs_on_construct(ecs_Registry* self, ecs_size_t type_id);
Signal* ecs_on_destruct(ecs_Registry* self, ecs_size_t type_id);
#endif // WORLD_H
