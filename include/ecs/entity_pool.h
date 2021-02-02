#ifndef ECS_ENTITY_POOL_H
#define ECS_ENTITY_POOL_H
#include "common.h"
#include <SDL2/SDL_stdinc.h>
#include "toolbox/toolbox.h"
typedef struct EcsEntityPool
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_size_t    size;
  ecs_size_t    destroyed_index;
} EcsEntityPool;

EcsEntityPool* ecs_entity_pool_init(EcsEntityPool* self);
void           ecs_entity_pool_fini(EcsEntityPool* self);

/*allocate new entity grow pool if needed*/
ecs_entity_t ecs_entity_pool_alloc_ent(EcsEntityPool* self);

/*free given entity and increment version by 1*/
void ecs_entity_pool_free_ent(EcsEntityPool* self, ecs_entity_t e);

INLINE BOOL ecs_entity_pool_is_valid(EcsEntityPool* self, ecs_entity_t e)
{
  ecs_size_t idx = ECS_ENT_IDX(e);

  return (idx < self->size) && (self->entities[idx] == e);
}
#endif // ECS_ENTITY_POOL_H
