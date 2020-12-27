#include "system/equipment_sys.h"
#include <components.h>

extern Ecs* g_ecs;

void equipment_system_update()
{
  ecs_entity_t* ett;
  Equipment*    equiments;
  Transform*    transform;
  Visual*       visual;
  Transform*    weapon_transform;
  Visual*       weapon_visual;
  ecs_size_t    cnt;
  ecs_entity_t  weapon;
  int           sign;

  ecs_raw(g_ecs, EQUIPMENT, &ett, (void**)&equiments, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    weapon = equiments[i].weapon;
    if (weapon == ECS_NULL_ENT)
      continue;

    transform = ecs_get(g_ecs, ett[i], TRANSFORM);
    visual    = ecs_get(g_ecs, ett[i], VISUAL);

    weapon_transform = ecs_get(g_ecs, weapon, TRANSFORM);
    weapon_visual    = ecs_get(g_ecs, weapon, VISUAL);

    sign = FLIP_TO_SIGN(visual->flip);

    weapon_transform->pos.x       = transform->pos.x + sign * equiments[i].weapon_anchor.x;
    weapon_transform->pos.y       = transform->pos.y + equiments[i].weapon_anchor.y;
    weapon_transform->was_changed = TRUE;

    weapon_visual->flip = visual->flip;
  }
}
