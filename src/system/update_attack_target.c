#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"

void update_attack_target_system(void)
{
  extern Ecs*   g_ecs;
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Vec2          target_position;
  AttackTarget* attack_target;
  Spot*         spot;
  ecs_entity_t  player;
  Vec2          player_position;

  ecs_raw(g_ecs, SPOT, &entities, (void**)&spot, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((attack_target = ecs_get(g_ecs, entities[i], ATTACK_TARGET)))
    {
      target_position = get_entity_position(g_ecs, attack_target->value);
      if (vec2_dist(target_position, spot[i].position) > spot[i].radius)
      {
        ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
      }
    }
    else if ((player = get_player(g_ecs)) != ECS_NULL_ENT)
    {
      player_position = get_entity_position(g_ecs, player);
      if (vec2_dist(player_position, spot[i].position) <= spot[i].radius)
      {
        ecs_add_w_data(g_ecs, entities[i], ATTACK_TARGET, &(AttackTarget){ player });
      }
    }
  }
}
