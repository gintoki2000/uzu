#ifndef ECS_POOL_H
#define ECS_POOL_H
#include "ecs/common.h"
#include "toolbox/toolbox.h"

typedef struct ecs_Pool
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

} ecs_Pool;

ecs_Pool* ecs_pool_create(ecs_size_t type_size);
void     ecs_pool_free(ecs_Pool* pool);

ecs_Pool* ecs_pool_init(ecs_Pool* self, ecs_size_t type_size);
void     ecs_pool_fini(ecs_Pool* self);

void* ecs_pool_add(ecs_Pool* self, ecs_entity_t e);
void  ecs_pool_rmv(ecs_Pool* self, ecs_entity_t e);
void* ecs_pool_get(ecs_Pool* self, ecs_entity_t e);
void  ecs_pool_swp(ecs_Pool* self, ecs_entity_t e1, ecs_entity_t e2);

ecs_size_t ecs_pool_count(ecs_Pool* self);
SDL_bool   ecs_pool_contains(ecs_Pool* self, ecs_entity_t e);

#endif // ECS_POOL_H
