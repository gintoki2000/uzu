#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;

static Vec2 get_mouse_position(void)
{
  Vec2 mouse_position;
  int  x, y;
  SDL_GetMouseState(&x, &y);
  mouse_position.x = x / SCL_X;
  mouse_position.y = y / SCL_Y;
  return mouse_position;
}

static void update_player_facing_direction(void)
{
  ecs_entity_t     player = get_player(g_ecs);
  Vec2             mouse_position;
  Vec2             player_position;
  Vec2             direction;
  FacingDirection* facing_direction;
  if (player != ECS_NULL_ENT && (facing_direction = ecs_get(g_ecs, player, FACING_DIRECTION)))
  {
    if (!facing_direction->frezze)
    {
      mouse_position  = get_mouse_position();
      player_position = get_entity_position(g_ecs, player);
      direction       = vec2_unit(vec2_sub(mouse_position, player_position));
    }
  }
}

static void update_facing_direction_by_attack_target(void)
{
  ecs_entity_t* entities;
  ecs_size_t    count;

  FacingDirection*    facing_direction;
  Transform *         self_transform, *target_transform;
  const AttackTarget* attack_target;
  Vec2                relative_position;
  const Motion*       motion;

  ecs_raw(g_ecs, FACING_DIRECTION, &entities, (void**)&facing_direction, &count);
  for (int i = 0; i < count; ++i)
  {
    if (!facing_direction->frezze)
    {
      if ((attack_target = ecs_get(g_ecs, entities[i], ATTACK_TARGET)) &&
          (attack_target->value != ECS_NULL_ENT))
      {
        target_transform = ecs_get(g_ecs, attack_target->value, TRANSFORM);
        self_transform   = ecs_get(g_ecs, entities[i], TRANSFORM);

        relative_position         = vec2_sub(target_transform->position, self_transform->position);
        facing_direction[i].value = vec2_unit(relative_position);
      }
      else
      {
        // không có mục tiêu tấn công facing_direction = speed
        motion = ecs_get(g_ecs, entities[i], MOTION);
        if (motion && vec2_mag(motion->vel) > 0.1f)
        {
          facing_direction[i].value = vec2_unit(motion->vel);
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
