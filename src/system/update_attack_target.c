#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

static void on_entity_died(SDL_UNUSED void* arg, EntityDiedMsg* event)
{
  ecs_entity_t* entities;
  ecs_size_t    entityCount;
  AttackTarget* attackTarget;
  ecs_raw(g_ecs, ATTACK_TARGET, &entities, (void**)&attackTarget, &entityCount);
  for (int i = entityCount - 1; i >= 0; --i)
  {
    if (attackTarget[i].value == event->entity)
      ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
  }
}

void update_attack_target_system_init()
{
  ems_connect(MSG_ENTITY_DIED, CALLBACK_2(on_entity_died));
}

void update_attack_target_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    entityCount;
  AttackTarget* attackTarget;
  AggroArea*    aggroArea;
  ecs_entity_t  player;
  Vec2          playerPosition;

  ecs_raw(g_ecs, AGGRO_AREA, &entities, (void**)&aggroArea, &entityCount);
  for (int i = entityCount - 1; i >= 0; --i)
  {
    if ((attackTarget = ecs_get(g_ecs, entities[i], ATTACK_TARGET)))
    {
      if (!ecs_is_valid(g_ecs, attackTarget->value) ||
          vec2_dist(ett_get_position(g_ecs, attackTarget->value), aggroArea[i].position) >
              aggroArea[i].radius)

      {
        ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
      }
    }
    else if ((player = scn_get_player(g_ecs)) != ECS_NULL_ENT)
    {
      playerPosition = ett_get_position(g_ecs, player);
      if (vec2_dist(playerPosition, aggroArea[i].position) <= aggroArea[i].radius)
      {
        ecs_set(g_ecs, entities[i], ATTACK_TARGET, &(AttackTarget){ player });
      }
    }
  }
}
