#include "following_system.h"
#include <components.h>
#include <utils.h>

extern Ecs* g_ecs;
void        following_system_update(void)
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  FollowingTarget* target;
  Transform*       transform;
  Motion*          motion;
  Vec2             target_pos;
  Vec2             desired;

  ecs_raw(g_ecs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {

    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
        (motion = ecs_get(g_ecs, entities[i], MOTION)))
    {
      target_pos = get_entity_position(g_ecs, target[i].entity);
      desired    = vec2_sub(target_pos, transform->pos);
      float dist = vec2_normalize(&desired);
      if (dist < target[i].radius)
      {
        ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
        motion->vel = VEC2_ZERO;
        continue;
      }
      desired     = vec2_mul(desired, motion->max_speed);
      motion->acc = vec2_trunc(vec2_sub(desired, motion->vel), motion->max_force);
    }
  }
}
