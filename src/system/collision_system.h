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
  ecs_entity_t e1;
  ecs_entity_t e2;
} CollisionPair;

void collision_system_init();
void collision_system_fini();
void collision_system_connect(int signal, pointer_t user_data, sig_handler_fn_t handler);
void CollisionSystem(Ecs* ecs);
#endif // COLLISION_SYSTEM_H
