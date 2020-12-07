#include "swinging_system.h"
#include "mediator.h"
#include <components.h>

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
void SwingingSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Swingable*    swingables;
  DamageOutput* damage_output;
  WeaponCore*   weapon_core;
  Visual*       visual;
  WeaponAction* weapon_action;
  Transform*    transform;

  ecs_data(ecs, SWINGABLE, &entities, (void**)&swingables, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (swingables[i].is_active)
    {
      if (++swingables[i].timer == SWINGING_SYSTEM_INTERVAL)
      {
        swingables[i].timer = 0;
        if (++swingables[i].step == SWINGING_SYSTEM_STEP)
        {
          if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
              (damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT)) &&
              (weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)))
          {

            weapon_core->in_action  = FALSE;
            damage_output->atk      = 0;
            transform->rot          = 0.0;
            transform->was_changed  = true;
            swingables[i].is_active = FALSE;
          }
        }
        else
        {
          if ((visual = ecs_get(ecs, entities[i], VISUAL)) &&
              (transform = ecs_get(ecs, entities[i], TRANSFORM)))
          {

            transform->rot         = swingables->step * 20.0 * FLIP_TO_SIGN(visual->flip);
            transform->was_changed = true;
          }
        }
      }
    }
    else
    {
      weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE);
      if (!weapon_core->in_action)
      {
        weapon_action = ecs_get(ecs, entities[i], WEAPON_ACTION);
        if (weapon_action->action == swingables[i].on_action)
        {
          damage_output = ecs_get(ecs, entities[i], DAMAGE_OUTPUT);

          swingables[i].timer     = 0;
          swingables[i].is_active = TRUE;
          swingables[i].step      = 0;
          damage_output->atk      = weapon_core->atk * SWINGING_SYSTEM_DAMAGE_ADJ;
          damage_output->type     = DAMAGE_TYPE_STRIKE;
          weapon_core->in_action  = TRUE;
          weapon_action->action   = WEAPON_ACTION_NONE;
        }
      }
    }
  }
}
