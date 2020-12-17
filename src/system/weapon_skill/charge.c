#include "charge.h"
#include <components.h>

#define SYS_WEAPON_SKILL_CHARGE_TIME 60

void sys_wpskl_charge_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponCore*   weapon_core;
  Controller*   wearer_controller;
  Visual*       visual;
  Motion*       weaer_motion;
  wpskl_Charge* wpskl_charges;
  Transform*    transform;
  DamageOutput* damage_output;

  ecs_data(ecs, WEAPON_SKILL_CHARGE, &entities, (void**)&wpskl_charges, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (wpskl_charges[i].is_active)
    {
      if (++wpskl_charges[i].timer == SYS_WEAPON_SKILL_CHARGE_TIME)
      {
        if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
            (weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
            (weaer_motion = ecs_get(ecs, weapon_core->wearer, MOTION)) &&
            (wearer_controller = ecs_get(ecs, weapon_core->wearer, CONTROLLER)) &&
            (damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT)))
        {
          transform->rot               = 0.0;
          transform->was_changed       = TRUE;
          weaer_motion->vel            = VEC2_ZERO;
          wearer_controller->in_action = FALSE;
          wearer_controller->lock_dir  = FALSE;
          wpskl_charges[i].is_active   = FALSE;
          damage_output->atk           = 0;
        }
      }
      else if ((weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
               (weaer_motion = ecs_get(ecs, weapon_core->wearer, MOTION)) &&
               (visual = ecs_get(ecs, entities[i], VISUAL)))
      {
        weaer_motion->acc = mulv(VEC2_RIGHT, FLIP_TO_SIGN(visual->flip) * weaer_motion->max_force);
      }
    }
    else if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
             (weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
             (weaer_motion = ecs_get(ecs, weapon_core->wearer, MOTION)) &&
             (wearer_controller = ecs_get(ecs, weapon_core->wearer, CONTROLLER)) &&
             (visual = ecs_get(ecs, entities[i], VISUAL)) &&
             (damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT)) &&
             (weaer_motion = ecs_get(ecs, weapon_core->wearer, MOTION)))
    {
      if (wearer_controller->action == wpskl_charges[i].on_action)
      {
        transform->rot               = 90.0 * FLIP_TO_SIGN(visual->flip);
        transform->was_changed       = TRUE;
        damage_output->atk           = weapon_core->atk;
        damage_output->type          = DAMAGE_TYPE_THUST;
        wearer_controller->action    = CHARACTER_ACTION_NONE;
        wearer_controller->lock_dir  = TRUE;
        wearer_controller->in_action = TRUE;
        wpskl_charges[i].is_active   = TRUE;
        wpskl_charges[i].timer       = 0;
        weaer_motion->acc = mulv(VEC2_RIGHT, FLIP_TO_SIGN(visual->flip) * weaer_motion->max_force);
      }
    }
  }
}
