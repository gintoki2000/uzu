#include "thust.h"
#include <components.h>
#include <ecs/ecs.h>

extern Ecs* g_ecs;

void weapon_skill_thust_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponBase*   base;
  wpskl_Thust*  skl;
  Controller*   wearer_controller;
  Equipment*    wearer_equipment;
  Transform*    transform;
  Motion*       wearer_motion;
  DamageOutput* damage_output;


  ecs_raw(g_ecs, WEAPON_SKILL_THUST, &entities, (pointer_t*)&skl, &cnt);
  for (ecs_size_t i = 0; i < cnt; ++i)
  {
    if ((base = ecs_get(g_ecs, entities[i], WEAPON_BASE)))
      switch (skl[i].state)
      {
      case 0: // INACTIVE
        if ((wearer_controller = ecs_get(g_ecs, base->wearer, CONTROLLER)))
        {
          if (wearer_controller->action == skl[i].on_action)
          {
            wearer_equipment = ecs_get(g_ecs, base->wearer, EQUIPMENT);
            wearer_motion    = ecs_get(g_ecs, base->wearer, MOTION);
            transform        = ecs_get(g_ecs, entities[i], TRANSFORM);
            damage_output    = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT);

            wearer_equipment->d.x            = -10;
            wearer_equipment->d.y            = -5;
            wearer_controller->in_action     = TRUE;
            wearer_controller->action        = CHARACTER_ACTION_NONE;
            wearer_controller->lock_movement = TRUE;
            wearer_motion->vel               = VEC2_ZERO;
            damage_output->atk               = 2;
            damage_output->type              = DAMAGE_TYPE_THUST;

            transform->rot = -15.0;

            skl[i].state = 1;
            skl[i].timer = 2;
          }
        }
        break;
      case 1:
        if (skl[i].timer && --skl[i].timer == 0)
        {
          wearer_equipment = ecs_get(g_ecs, base->wearer, EQUIPMENT);
          transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

          wearer_equipment->d.x = -8;
          wearer_equipment->d.y = -4;
          transform->rot        = -12;
          skl[i].state          = 2;
          skl[i].timer          = 2;
        }
        break;
      case 2:
        if (skl[i].timer && --skl[i].timer == 0)
        {
          wearer_equipment = ecs_get(g_ecs, base->wearer, EQUIPMENT);
          transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

          wearer_equipment->d.x = 10;
          wearer_equipment->d.y = 0;
          transform->rot        = 0.0;
          skl[i].state          = 3;
          skl[i].timer          = 7;
        }
        break;
      case 3:
        if ((wearer_controller = ecs_get(g_ecs, base->wearer, CONTROLLER)))
        {
          if (skl[i].timer && --skl[i].timer == 0)
          {
            wearer_equipment = ecs_get(g_ecs, base->wearer, EQUIPMENT);
            damage_output    = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT);

            wearer_controller->in_action     = FALSE;
            wearer_controller->lock_movement = FALSE;
            wearer_equipment->d.x            = 0;
            wearer_equipment->d.y            = 0;
            skl[i].state                     = 0;
            damage_output->atk               = 0;
          }
        }
      }
  }
}
