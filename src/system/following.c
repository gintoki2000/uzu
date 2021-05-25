#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"

void following_system(void)
{
  extern Ecs*       gEcs;
  ecs_entity_t*     entities;
  ecs_size_t        cnt;
  FollowingTarget*  target;
  Transform*        transform;
  Vec2              target_pos;
  Vec2              desired;
  DesiredDirection* desiredDirection;
  float             r;
  float             d;

  ecs_raw(gEcs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (!ecs_is_valid(gEcs, target[i].entity) &&
        (desiredDirection = ecs_get(gEcs, entities[i], DESIRED_DIRECTION)))
    {
      ecs_rmv(gEcs, entities[i], FOLLOWING_TARGET);
      *desiredDirection = VEC2_ZERO;
      continue;
    }

    if ((transform = ecs_get(gEcs, entities[i], TRANSFORM)) &&
        (desiredDirection = ecs_get(gEcs, entities[i], DESIRED_DIRECTION)))
    {
      target_pos = ett_get_position(gEcs, target[i].entity);
      desired    = vec2_sub(target_pos, transform->position);

      d       = vec2_mag2(desired);
      desired = vec2_unit(desired);
      r       = target[i].radius;
      if (d < r * r)
      {
        ecs_rmv(gEcs, entities[i], FOLLOWING_TARGET);
        *desiredDirection = VEC2_ZERO;
        continue;
      }

      *desiredDirection = desired;
    }
  }
}
