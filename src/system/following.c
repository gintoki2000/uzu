#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"

void following_system(void)
{
  extern Ecs*       g_ecs;
  ecs_entity_t*     entities;
  ecs_size_t        cnt;
  FollowingTarget*  target;
  Transform*        transform;
  Vec2              target_pos;
  Vec2              desired;
  DesiredDirection* desired_direction;
  float             r;
  float             d;

  ecs_raw(g_ecs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (!ecs_is_valid(g_ecs, target[i].entity) &&
        (desired_direction = ecs_get(g_ecs, entities[i], DESIRED_DIRECTION)))
    {
      ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
      *desired_direction = VEC2_ZERO;
      continue;
    }

    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
        (desired_direction = ecs_get(g_ecs, entities[i], DESIRED_DIRECTION)))
    {
      target_pos = ett_get_position(g_ecs, target[i].entity);
      desired    = vec2_sub(target_pos, transform->position);

      d       = vec2_mag2(desired);
      desired = vec2_unit(desired);
      r       = target[i].radius;
      if (d < r * r)
      {
        ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
        *desired_direction = VEC2_ZERO;
        continue;
      }

      *desired_direction = desired;
    }
  }
}
