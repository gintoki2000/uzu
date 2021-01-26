#include "swing.h"
#include "../event_messaging_sys.h"
#include <components.h>

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
extern Ecs* g_ecs;

void swing_weapon_skl_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_Swing*  skl;
  DamageOutput* damage_output;
  WeaponBase*   base;
  Visual*       visual;
  Transform*    transform;
  Controller*   wearer_controller;

  ecs_raw(g_ecs, WEAPON_SKILL_SWING, &entities, (void**)&skl, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (skl[i].is_active)
    {
      if (++skl[i].timer == SWINGING_SYSTEM_INTERVAL)
      {
        skl[i].timer = 0;
        if (++skl[i].step == SWINGING_SYSTEM_STEP)
        {
          if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
              (damage_output = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT)) &&
              (base = ecs_get(g_ecs, entities[i], WEAPON_BASE)) &&
              (wearer_controller = ecs_get(g_ecs, base->wearer, CONTROLLER)))
          {

            wearer_controller->in_action = FALSE;
            damage_output->atk           = 0;
            transform->rot               = 0.0;
            skl[i].is_active             = FALSE;
          }
        }
        else
        {
          if ((visual = ecs_get(g_ecs, entities[i], VISUAL)) &&
              (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
          {
            transform->rot = skl[i].step * 20.0 * FLIP_TO_SIGN(visual->flip);
          }
        }
      }
    }
    else
    {
      if ((base = ecs_get(g_ecs, entities[i], WEAPON_BASE)) &&
          (wearer_controller = ecs_get(g_ecs, base->wearer, CONTROLLER)) &&
          (damage_output = ecs_get(g_ecs, entities[i], DAMAGE_OUTPUT)))
      {

        if (!wearer_controller->in_action && (skl[i].on_action == wearer_controller->action))
        {
          Vec2 v = { 1.f, 0.f };

          skl[i].timer                 = 0;
          skl[i].is_active             = TRUE;
          skl[i].step                  = 0;
          damage_output->atk           = base->atk * SWINGING_SYSTEM_DAMAGE_ADJ;
          damage_output->type          = DAMAGE_TYPE_STRIKE;
          damage_output->impact        = TRUE;
          damage_output->force         = v;
          wearer_controller->action    = CHARACTER_ACTION_NONE;
          wearer_controller->in_action = TRUE;
        }
      }
    }
  }
}
