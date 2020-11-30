#ifndef WORLD_H
#define WORLD_H
#include "common.h"
#include "entity_pool.h"
#include "pool.h"
#include <SDL2/SDL.h>

typedef struct Ecs
{
  ecs_size_t    type_cnt;
  EcsType*      types;
  EcsPool**     pools;
  EcsEntityPool entity_pool;
} Ecs;

typedef struct EcsFilter
{
  const ecs_size_t* required;
  ecs_size_t        rcnt;
  const ecs_size_t* exclude;
  ecs_size_t        ecnt;
} EcsFilter;

typedef void (*ecs_each_fn_t)(void* user_data, Ecs* ecs, ecs_entity_t entity);
typedef void (*ecs_each_ex_fn_t)(void* user_data, Ecs* ecs, ecs_entity_t entity, void** components);

Ecs* ecs_new(const EcsType* types, ecs_size_t cnt);
void ecs_del(Ecs* ecs);

Ecs* ecs_init(Ecs* self, const EcsType* types, ecs_size_t type_cnt);
void ecs_fini(Ecs* self);

ecs_entity_t ecs_create(Ecs* self);
void         ecs_destroy(Ecs* self, ecs_entity_t entity);
void*        ecs_add(Ecs* self, ecs_entity_t entity, ecs_size_t component_type);
void* ecs_add_w_data(Ecs* self, ecs_entity_t entity, ecs_size_t component_type, const void* data);
void  ecs_rmv(Ecs* self, ecs_entity_t entity, ecs_size_t component_type);
void* ecs_get(Ecs* self, ecs_entity_t entity, ecs_size_t component_type);
SDL_bool ecs_has(Ecs* self, ecs_entity_t entity, ecs_size_t component_type);
void     ecs_rmv_all(Ecs* self, ecs_entity_t entity);
void     ecs_each(Ecs* self, void* user_data, ecs_each_fn_t each_fn);
void     ecs_each_w_filter(Ecs*             self,
                           const EcsFilter* filter,
                           void*            user_data,
                           ecs_each_ex_fn_t each_fn);
SDL_bool ecs_has_ex(Ecs* self, const ecs_size_t* types, ecs_size_t cnt);
void     ecs_data(Ecs*           self,
                  ecs_size_t     type,
                  ecs_entity_t** entities_ptr,
                  void**         components_ptr,
                  ecs_size_t*    cnt_ptr);

static SDL_bool ecs_is_valid(Ecs* self, ecs_entity_t entity)
{
  return ecs_entity_pool_is_valid(&self->entity_pool, entity);
}
#endif // WORLD_H
