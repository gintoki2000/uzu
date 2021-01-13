#include "casting_sys.h"
#include "components.h"
#include "ecs/ecs.h"
#include "types.h"

extern const Spell g_spell_tbl[];
extern Ecs*        g_ecs;

void casting_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Castable*       castables;
  Transform*      transform;
  WeaponBase*     weapon_core;
  Controller*     wearer_controller;
  AttunementSlot* attunement_slot;
  ManaPool*       mana_pool;
  const Spell* spell;

  ecs_raw(g_ecs, CASTABLE, &entities, (pointer_t*)&castables, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (castables[i].timer)
    {
      --castables[i].timer;
      // just finish
      if (castables[i].timer == 0)
      {
        if ((weapon_core       = ecs_get(g_ecs, entities[i],         WEAPON_BASE)) &&
            (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)))
        {
          wearer_controller->in_action = FALSE;
        }
      }
    }

    if (castables[i].timer == 0 && 
       (weapon_core       = ecs_get(g_ecs, entities[i],         WEAPON_BASE)) &&
       (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)) &&
       (attunement_slot   = ecs_get(g_ecs, weapon_core->wearer, ATTUNEMENT_SLOT)) &&
       (mana_pool         = ecs_get(g_ecs, weapon_core->wearer, MANA_POOL)))
    {
      spell = &g_spell_tbl[attunement_slot->spell_id];
      if (wearer_controller->action == CHARACTER_ACTION_REGULAR_ATK && 
          mana_pool->mana_points >= spell->cost) 
      {
        wearer_controller->action    = CHARACTER_ACTION_NONE;
        wearer_controller->in_action = TRUE;
        castables[i].timer           = spell->cast_spd;
        spell->cast_fn(g_ecs, weapon_core->wearer);
        
      }
    }
  }
}
