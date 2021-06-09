#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "toolbox/toolbox.h"

extern ecs_Registry* gRegistry;

static Vec2 get_mouse_position(void)
{
  Vec2        mousePosition;
  extern RECT gViewport;
  int         x, y;
  SDL_GetMouseState(&x, &y);
  mousePosition.x = x / SCL_X;
  mousePosition.y = y / SCL_Y;
  mousePosition.x += gViewport.x;
  mousePosition.y += gViewport.y;
  return mousePosition;
}

static void update_player_facing_direction(void)
{
  ecs_entity_t     player = scn_get_player(gRegistry);
  Vec2             mousePosition;
  Vec2             playerPosition;
  AimDirection*    facingDirection;

  if (player != ECS_NULL_ENT && (facingDirection = ecs_get(gRegistry, player, AIM_DIRECTION)))
  {
    if (!facingDirection->frezze)
    {
      mousePosition          = get_mouse_position();
      playerPosition         = ett_get_position(gRegistry, player);
      facingDirection->value = vec2_unit(vec2_sub(mousePosition, playerPosition));
    }
  }
}

static void update_facing_direction_by_attack_target(void)
{
  ecs_entity_t* entities;
  ecs_size_t    count;

  AimDirection*       facingDirection;
  Transform *         selfTransform, *targetTransform;
  const AttackTarget* attackTarget;
  Vec2                relativePosition;
  const Motion*       motion;

  ecs_raw(gRegistry, AIM_DIRECTION, &entities, (void**)&facingDirection, &count);
  for (int i = 0; i < count; ++i)
  {
    if (ecs_has(gRegistry, entities[i], PLAYER_TAG))
      continue;
    if (!facingDirection->frezze)
    {
      if ((attackTarget = ecs_get(gRegistry, entities[i], ATTACK_TARGET)) &&
          (attackTarget->value != ECS_NULL_ENT))
      {
        targetTransform = ecs_get(gRegistry, attackTarget->value, TRANSFORM);
        selfTransform   = ecs_get(gRegistry, entities[i], TRANSFORM);

        relativePosition         = vec2_sub(targetTransform->position, selfTransform->position);
        facingDirection[i].value = vec2_unit(relativePosition);
      }
      else
      {
        // không có mục tiêu tấn công facingDirection = speed
        motion = ecs_get(gRegistry, entities[i], MOTION);
        if (motion && vec2_mag(motion->vel) > 0.1f)
        {
          facingDirection[i].value = vec2_unit(motion->vel);
        }
      }
    }
  }
}

void update_facing_direction_system(void)
{
  update_player_facing_direction();
  update_facing_direction_by_attack_target();
}
