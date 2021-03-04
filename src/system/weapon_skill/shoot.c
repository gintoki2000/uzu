#include "components.h"
#include "entity_factory.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void weapon_shoot_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponShoot* shoots;

  Controller*            controller;
  Holder*                holder;
  AttackMask*            attack_mask;
  Transform*             transform;
  const FacingDirection* facing_direction;
  Vec2                   projspd;

  ecs_raw(g_ecs, WEAPON_SHOOT, &entities, (void**)&shoots, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (shoots[i].timer && --shoots[i].timer)
    {
      if ((holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
          (controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
      {
        controller->in_action = FALSE;
      }
    }

    if (!shoots[i].timer)
    {
      if ((holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
          (controller = ecs_get(g_ecs, holder->value, CONTROLLER)) &&
          (attack_mask = ecs_get(g_ecs, holder->value, ATTACK_MASK)) &&
          (transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
          (facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION)))
      {
        if (!controller->in_action && controller->action == shoots[i].on_action)
        {
          shoots[i].timer       = shoots[i].fire_rate;
          controller->in_action = TRUE;
          controller->action    = CHARACTER_ACTION_NONE;
          projspd               = vec2_mul(facing_direction->value, 170.f);
          make_arrow(g_ecs, holder->value, transform->position, projspd, attack_mask->value);
        }
      }
    }
  }
}
