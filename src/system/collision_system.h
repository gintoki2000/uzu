#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>


typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} CollisionPair;

void collision_system_init(Ecs* ecs);
void collision_system_fini();
void collision_system_draw_debug(SDL_Renderer* renderer);
void collision_system_query(const AABB* aabb, Callback callback);

void CollisionSystem(Ecs* ecs);
#endif // COLLISION_SYSTEM_H
