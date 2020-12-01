#include "entity_pool.h"
#include "common.h"
#include <SDL2/SDL_assert.h>
#include <stdlib.h>

#define ECS_ENTITY_POOL_DEFAULT_SIZE 16

EcsEntityPool* ecs_entity_pool_init(EcsEntityPool* self)
{
  self->cnt = 0;
  self->size = ECS_ENTITY_POOL_DEFAULT_SIZE;
  self->destroyed_index = 0;
  self->entities = malloc(ECS_ENTITY_POOL_DEFAULT_SIZE * sizeof(ecs_entity_t));

  for (int i = 0; i < ECS_ENTITY_POOL_DEFAULT_SIZE - 1; ++i)
  {
    self->entities[i] = ECS_ENT(i + 1, 0);
  }
  self->entities[ECS_ENTITY_POOL_DEFAULT_SIZE - 1] = ECS_ENT(ECS_NULL_IDX, 0);
  return self;
}

void ecs_entity_pool_fini(EcsEntityPool* self)
{
  if (self != NULL)
  {
    free(self->entities);
  }
}

ecs_entity_t ecs_entity_pool_alloc_ent(EcsEntityPool* self)
{
  ecs_size_t ver;
  ecs_size_t idx;

  if (self->destroyed_index == ECS_NULL_IDX)
  {
    ecs_size_t old_size = self->size;
    self->size *= 2;
    self->entities = realloc(self->entities, self->size * sizeof(ecs_entity_t));
    for (int i = old_size; i < self->size - 1; ++i)
    {
      self->entities[i] = ECS_ENT(i + 1, 0);
    }
    self->entities[self->size - 1] = ECS_ENT(ECS_NULL_IDX, 0);
    self->destroyed_index = old_size;
  }

  idx = self->destroyed_index;
  ver = ECS_ENT_VER(self->entities[idx]);
  self->destroyed_index = ECS_ENT_IDX(self->entities[idx]);
  self->entities[idx] = ECS_ENT(idx, ver);

  return ECS_ENT(idx, ver);
}

void ecs_entity_pool_free_ent(EcsEntityPool* self, ecs_entity_t e)
{
  ecs_size_t ent_ver;
  ecs_size_t ent_idx;

  ent_ver = ECS_ENT_VER(e);
  ent_idx = ECS_ENT_IDX(e);

  self->entities[ent_idx] = ECS_ENT(self->destroyed_index, ent_ver + 1);
  self->destroyed_index = ent_idx;
}

