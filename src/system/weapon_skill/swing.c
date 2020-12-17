#include "swing.h"
#include "../mediator.h"
#include <components.h>

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
void sys_wpskl_swing_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_Swing*  wpskl_swing;
  DamageOutput* damage_output;
  WeaponCore*   weapon_core;
  Visual*       visual;
  Transform*    transform;
  Controller*   wearer_controller;

  ecs_data(ecs, WEAPON_SKILL_SWING, &entities, (void**)&wpskl_swing, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (wpskl_swing[i].is_active)
    {
      if (++wpskl_swing[i].timer == SWINGING_SYSTEM_INTERVAL)
      {
        wpskl_swing[i].timer = 0;
        if (++wpskl_swing[i].step == SWINGING_SYSTEM_STEP)
        {
          if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
              (damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT)) &&
              (weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
              (wearer_controller = ecs_get(ecs, weapon_core->wearer, CONTROLLER)))
          {

            wearer_controller->in_action = FALSE;
            damage_output->atk           = 0;
            transform->rot               = 0.0;
            transform->was_changed       = true;
            wpskl_swing[i].is_active     = FALSE;
          }
        }
        else
        {
          if ((visual = ecs_get(ecs, entities[i], VISUAL)) &&
              (transform = ecs_get(ecs, entities[i], TRANSFORM)))
          {

            transform->rot         = wpskl_swing[i].step * 20.0 * FLIP_TO_SIGN(visual->flip);
            transform->was_changed = true;
          }
        }
      }
    }
    else
    {
      if ((weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
          (wearer_controller = ecs_get(ecs, weapon_core->wearer, CONTROLLER)) &&
          (damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT)))
      {

        if (!wearer_controller->in_action &&
            (wpskl_swing[i].on_action == wearer_controller->action))
        {
          wpskl_swing[i].timer         = 0;
          wpskl_swing[i].is_active     = TRUE;
          wpskl_swing[i].step          = 0;
          damage_output->atk           = weapon_core->atk * SWINGING_SYSTEM_DAMAGE_ADJ;
          damage_output->type          = DAMAGE_TYPE_STRIKE;
          wearer_controller->action    = CHARACTER_ACTION_NONE;
          wearer_controller->in_action = TRUE;
        }
      }
    }
  }
}
