#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  SIG_PLAYER_COLLIDED_W_ENEMY,
  SIG_PLAYER_WEAPON_COLLIED_W_ENEMY,
  NUM_COLLISION_SIGS,
};

typedef struct
{
  ecs_entity_t player;
  ecs_entity_t enemy;
} PlayerHitEnemyEvent;

typedef struct
{
  ecs_entity_t enemy;
  ecs_entity_t weapon;
} WeaponHitEnemyEvent;

typedef struct
{
  ecs_entity_t projectile;
  ecs_entity_t enemy;
} ProjecttileHitEnemyEvent;

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} CollisionPair;

void collision_system_init(Ecs* ecs);
void collision_system_fini();
void collision_system_connect(int signal, pointer_t user_data, sig_handler_fn_t handler);
void collision_system_draw_debug(SDL_Renderer* renderer);
void collision_system_query(const AABB* aabb, Callback callback);

void CollisionSystem(Ecs* ecs);
#endif // COLLISION_SYSTEM_H
