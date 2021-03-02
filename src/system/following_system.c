#include "system/game_logic.h"

#include "components.h"
#include "entity_utils.h"

extern Ecs* g_ecs;

void following_system(void)
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  FollowingTarget* target;
  Transform*       transform;
  Controller*      controller;
  Vec2             target_pos;
  Vec2             desired;
  float            r;
  float            d;

  ecs_raw(g_ecs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (!ecs_is_valid(g_ecs, target[i].entity))
    {
      ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
      controller->desired_direction = VEC2_ZERO;
      continue;
    }

    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
        (controller = ecs_get(g_ecs, entities[i], CONTROLLER)))
    {
      target_pos = get_entity_position(g_ecs, target[i].entity);
      desired    = vec2_sub(target_pos, transform->position);

      d       = vec2_mag2(desired);
      desired = vec2_unit(desired);
      r       = target[i].radius;
      if (d < r * r)
      {
        ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
        controller->desired_direction = VEC2_ZERO;
        continue;
      }

      controller->desired_direction = desired;
    }
  }
}
