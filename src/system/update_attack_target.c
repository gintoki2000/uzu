#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"

void update_attack_target_system(void)
{
  extern Ecs*   g_ecs;
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Vec2          target_position;
  Vec2          relative_position;
  AttackTarget* attack_target;
  Spot*         spot;
  ecs_entity_t  player;
  Vec2          player_position;

  ecs_raw(g_ecs, ATTACK_TARGET, &entities, (void**)&spot, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((attack_target = ecs_get(g_ecs, entities[i], ATTACK_TARGET)))
    {
      target_position   = get_entity_position(g_ecs, attack_target->value);
      relative_position = vec2_sub(target_position, spot[i].position);
      if (vec2_mag2(relative_position) > (spot->radius * spot->radius))
      {
        ecs_rmv(g_ecs, entities[i], ATTACK_TARGET);
      }
    }
    else if ((player = get_player(g_ecs)) != ECS_NULL_ENT)
    {
      player_position   = get_entity_position(g_ecs, player);
      relative_position = vec2_sub(player_position, spot[i].position);
      if (vec2_mag(relative_position) <= spot[i].radius)
      {
        ecs_add(g_ecs, entities[i], player);
      }
    }
  }
}
