#ifndef ECS_POOL_H
#define ECS_POOL_H
#include "common.h"
#include <SDL2/SDL.h>

typedef struct EcsPool
{
  ecs_size_t type_size;
  struct
  {
    ecs_size_t  size;
    ecs_size_t* data;
  } sparse;

  struct
  {
    ecs_size_t    size;
    ecs_size_t    cnt;
    ecs_entity_t* entities;
    void*         components;
  } dense;

} EcsPool;

EcsPool* ecs_pool_new(ecs_size_t type_size);
void     ecs_pool_del(EcsPool* pool);

EcsPool* ecs_pool_init(EcsPool* self, ecs_size_t type_size);
void     ecs_pool_fini(EcsPool* self);

void  ecs_pool_add(EcsPool* self, ecs_entity_t e);
void  ecs_pool_rmv(EcsPool* self, ecs_entity_t e);
void* ecs_pool_get(EcsPool* self, ecs_entity_t e);
void  ecs_pool_swp(EcsPool* self, ecs_entity_t e1, ecs_entity_t e2);

ecs_size_t ecs_pool_cnt(EcsPool* self);
SDL_bool   ecs_pool_contains(EcsPool* self, ecs_entity_t e);

#endif // ECS_POOL_H
