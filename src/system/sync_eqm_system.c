#include "ecs/common.h"
#include "ecs/ecs.h"
#include <components.h>
#include <system/sync_eqm_system.h>

void SyncEqmSystem(Ecs* ecs)
{
  ecs_entity_t* ett;
  Equipment*    equiments;
  Transform*    transform;
  Visual*       visual;
  Transform*    weapon_transform;
  Visual*       weapon_visual;
  ecs_size_t    cnt;
  ecs_entity_t  weapon;

  ecs_data(ecs, EQUIPMENT, &ett, (void**)&equiments, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    weapon = equiments[i].rhand;
    if (weapon == ECS_NULL_ENT)
      continue;

    transform = ecs_get(ecs, ett[i], TRANSFORM);
    visual = ecs_get(ecs, ett[i], VISUAL);

    weapon_transform = ecs_get(ecs, weapon, TRANSFORM);
    weapon_visual = ecs_get(ecs, weapon, VISUAL);

    weapon_transform->pos.x = transform->pos.x + FLIP_TO_SIGN(visual->flip) * 6;
    weapon_transform->pos.y = transform->pos.y + 6;
    weapon_transform->was_changed = TRUE;

    weapon_visual->flip = visual->flip;
  }
}
