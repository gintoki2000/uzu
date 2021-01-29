#include "following_system.h"
#include <components.h>
#include <utils.h>

extern Ecs* g_ecs;

void following_system_update(void)
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  FollowingTarget* target;
  Transform*       transform;
  Motion*          motion;
  Controller*      controller;
  Vec2             target_pos;
  Vec2             desired;
  float            r;
  float            d;

  ecs_raw(g_ecs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {

    if ((transform  = ecs_get(g_ecs, entities[i], TRANSFORM )) &&
        (motion     = ecs_get(g_ecs, entities[i], MOTION    )) &&
        (controller = ecs_get(g_ecs, entities[i], CONTROLLER)))
    {
      target_pos = get_entity_position(g_ecs, target[i].entity);
      desired    = vec2_sub(target_pos, transform->position);

      d       = vec2_mag2(desired);
      desired = vec2_unit_vec(desired);
      r       = target[i].radius;
      if (d < r * r)
      {
        ecs_rmv(g_ecs, entities[i], FOLLOWING_TARGET);
        motion->vel = VEC2_ZERO;
        continue;
      }
      desired = vec2_mul(desired, motion->max_speed);
      desired = vec2_sub(desired, motion->vel);

      controller->force.x += desired.x;
      controller->force.y += desired.y;
    }
  }
}
