#include "ecs/pool.h"
#include "ecs/common.h"

#include <stdlib.h>
#include <string.h>

#define ECS_POOL_CONTAINS(pool, idx)                                                               \
  ((idx < pool->sparse.size) && (pool->sparse.data[idx] != ECS_NULL_IDX))
#define ECS_POOL_OFFSET(pool, idx)                                                                 \
  ((unsigned char*)pool->dense.components + ((idx)*pool->type_size))
#define ECS_POOL_SET(pool, e, cidx) (pool->sparse.data[ECS_ENT_IDX(e)] = cidx)
#define ECS_POOL_DFAULT_SIZE 16
static void swap_component_data(EcsPool* self, ecs_size_t cidx1, ecs_size_t cidx2)
{
  unsigned char  tmp[self->type_size];
  unsigned char* mem1;
  unsigned char* mem2;
  ecs_entity_t   etmp;

  mem1 = ECS_POOL_OFFSET(self, cidx1);
  mem2 = ECS_POOL_OFFSET(self, cidx2);

  SDL_memcpy(tmp, mem1, self->type_size);
  SDL_memcpy(mem1, mem2, self->type_size);
  SDL_memcpy(mem2, tmp, self->type_size);

  etmp                        = self->dense.entities[cidx1];
  self->dense.entities[cidx1] = self->dense.entities[cidx2];
  self->dense.entities[cidx2] = etmp;

  ECS_POOL_SET(self, self->dense.entities[cidx1], cidx1);
  ECS_POOL_SET(self, self->dense.entities[cidx2], cidx2);
}

static void realloc_sparse_array(EcsPool* self, ecs_size_t new_size)
{
  ecs_size_t old_size;

  old_size = self->sparse.size;
  while (self->sparse.size < new_size)
    self->sparse.size *= 2;

  self->sparse.data = SDL_realloc(self->sparse.data, self->sparse.size * sizeof(ecs_size_t));
  for (int i = old_size; i < self->sparse.size; ++i)
    self->sparse.data[i] = ECS_NULL_IDX;
}

static void realloc_dense_array(EcsPool* self, ecs_size_t new_size)
{
  ecs_size_t old_size    = self->dense.size;
  self->dense.size       = new_size;
  self->dense.entities   = SDL_realloc(self->dense.entities, new_size * sizeof(ecs_entity_t));
  self->dense.components = SDL_realloc(self->dense.components, new_size * self->type_size);
  for (int i = old_size; i < self->dense.size; ++i)
    self->dense.entities[i] = ECS_NULL_ENT;
}

EcsPool* ecs_pool_new(ecs_size_t type_size)
{
  return ecs_pool_init(SDL_malloc(sizeof(EcsPool)), type_size);
}

void ecs_pool_del(EcsPool* pool)
{
  ecs_pool_fini(pool);
  free(pool);
}

EcsPool* ecs_pool_init(EcsPool* self, ecs_size_t type_size)
{
  if (self != NULL)
  {
    self->type_size        = type_size;
    self->dense.cnt        = 0;
    self->dense.size       = ECS_POOL_DFAULT_SIZE;
    self->dense.components = SDL_malloc(ECS_POOL_DFAULT_SIZE * type_size);
    self->dense.entities   = SDL_malloc(ECS_POOL_DFAULT_SIZE * sizeof(ecs_entity_t));
    self->sparse.size      = ECS_POOL_DFAULT_SIZE;
    self->sparse.data      = SDL_malloc(ECS_POOL_DFAULT_SIZE * sizeof(ecs_size_t));

    for (int i = 0; i < ECS_POOL_DFAULT_SIZE; ++i)
      self->sparse.data[i] = ECS_NULL_IDX;

    for (int i = 0; i < ECS_POOL_DFAULT_SIZE; ++i)
      self->dense.entities[i] = ECS_NULL_ENT;
  }
  return self;
}

void ecs_pool_fini(EcsPool* self)
{
  if (self != NULL)
  {
    free(self->dense.components);
    free(self->dense.entities);
    free(self->sparse.data);
  }
}

void* ecs_pool_add(EcsPool* self, ecs_entity_t e)
{
  ecs_size_t idx = ECS_ENT_IDX(e);

  ASSERT(!ECS_POOL_CONTAINS(self, idx) && "already assigned");

  if (idx >= self->sparse.size)
    realloc_sparse_array(self, idx + 1);

  if (self->dense.cnt == self->dense.size)
    realloc_dense_array(self, self->dense.size * 2);

  self->sparse.data[idx]                    = self->dense.cnt++;
  self->dense.entities[self->dense.cnt - 1] = e;
  return ECS_POOL_OFFSET(self, self->dense.cnt - 1);
}

void ecs_pool_rmv(EcsPool* self, ecs_entity_t e)
{
  ecs_size_t eidx, cidx, last_cidx;

  eidx = ECS_ENT_IDX(e);

  ASSERT(eidx < self->sparse.size && "this pool does not contains given entity");

  cidx = self->sparse.data[eidx];

  ASSERT(cidx != ECS_NULL_IDX && "this pool does not contains given entity");

  last_cidx = self->dense.cnt - 1;
  swap_component_data(self, cidx, last_cidx);
  self->sparse.data[eidx] = ECS_NULL_IDX;
  self->dense.cnt--;
}

void* ecs_pool_get(EcsPool* self, ecs_entity_t e)
{
  ecs_size_t idx = ECS_ENT_IDX(e);
  return ECS_POOL_CONTAINS(self, idx) ? ECS_POOL_OFFSET(self, self->sparse.data[idx]) : NULL;
}

void ecs_pool_swp(EcsPool* self, ecs_entity_t e1, ecs_entity_t e2)
{
  swap_component_data(self, self->sparse.data[ECS_ENT_IDX(e1)], self->sparse.data[ECS_ENT_IDX(e2)]);
}

ecs_size_t ecs_pool_cnt(EcsPool* self)
{
  return self->dense.cnt;
}

SDL_bool ecs_pool_contains(EcsPool* self, ecs_entity_t e)
{
  return ECS_POOL_CONTAINS(self, ECS_ENT_IDX(e));
}
