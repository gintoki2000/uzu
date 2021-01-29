#include "system/casting_system.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/event_messaging_sys.h"
#include "types.h"

extern Ecs* g_ecs;

void casting_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Castable* castable;

  Controller*       wearer_controller;
  WeaponAttributes* attributes;
  AttunementSlot*   attunement_slot;
  ManaPool*         mana_pool;

  ecs_raw(g_ecs, CASTABLE, &entities, (void**)&castable, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (castable[i].timer > 0)
    {
      // cooldown
      --castable[i].timer;
    }
    else
    {
      if ((attributes = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
          (wearer_controller = ecs_get(g_ecs, attributes->wearer, CONTROLLER)) &&
          (attunement_slot = ecs_get(g_ecs, attributes->wearer, ATTUNEMENT_SLOT)) &&
          (mana_pool = ecs_get(g_ecs, attributes->wearer, MANA_POOL)))
      {
        const Spell* spell     = &g_spell_tbl[attunement_slot->spell_id];
        BOOL         bFire     = wearer_controller->action == CHARACTER_ACTION_REGULAR_ATK;
        BOOL         bEnoughMp = mana_pool->mana_points >= spell->cost;
        if (bFire && bEnoughMp)
        {
          mana_pool->mana_points -= spell->cost;
          spell->cast_fn(g_ecs, attributes->wearer, entities[i]);
          castable[i].timer = spell->cast_spd;
        }
      }
    }
  }
}
