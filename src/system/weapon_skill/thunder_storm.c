#include "thunder_storm.h"
#include <components.h>
#include <entity_factory.h>
#include <stdlib.h>

void sys_wpskl_thunder_storm_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_ThunderStorm* skills;
  WeaponCore*         weapon_core;
  Controller*         wearer_ctl;
  Transform*          transform;

  int rx, ry;

  ecs_data(ecs, WEAPON_SKILL_THUNDER_STORM, &entities, (void**)&skills, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (skills[i].remaining > 0)
    {
      if (++skills[i].timer == skills[i].interval)
      {
        skills[i].timer = 0;
        transform       = ecs_get(ecs, entities[i], TRANSFORM);
        rx              = rand() % 100 - 50 + transform->pos.x;
        ry              = rand() % 100 - 50 + transform->pos.y;
        make_thunder(ecs, VEC2(rx, ry), 0);
        if (--skills[i].remaining == 0)
        {
          weapon_core               = ecs_get(ecs, entities[i], WEAPON_CORE);
          wearer_ctl                = ecs_get(ecs, weapon_core->wearer, CONTROLLER);
          wearer_ctl->in_action     = FALSE;
        }
      }
    }
    if ((weapon_core = ecs_get(ecs, entities[i], WEAPON_CORE)) &&
        (wearer_ctl = ecs_get(ecs, weapon_core->wearer, CONTROLLER)) &&
        (transform = ecs_get(ecs, entities[i], TRANSFORM)))
    {
      if (wearer_ctl->action == skills[i].on_action)
      {
        wearer_ctl->action        = CHARACTER_ACTION_NONE;
        wearer_ctl->in_action     = TRUE;
        skills[i].timer           = 0;
        skills[i].remaining       = skills[i].total;
      }
    }
  }
}
