#include "system/weapon_skill/thunder_storm.h"
#include "components.h"
#include "entity_factory.h"
#include <stdlib.h>

extern Ecs* g_ecs;

void thunder_storm_weapon_skl_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_ThunderStorm* skills;
  WeaponAttributes*   attrs;
  Controller*         wearer_ctl;
  Transform*          transform;

  int rx, ry;

  ecs_raw(g_ecs, WEAPON_SKILL_THUNDER_STORM, &entities, (void**)&skills, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
  }
}
