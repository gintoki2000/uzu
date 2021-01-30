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

  Castable*         castables;
  WeaponAttributes* weapon_core;
  Controller*       wearer_controller;
  AttunementSlot*   attunement_slot;
  ManaPool*         mana_pool;
  const Spell*      spell;
  BOOL              trigger;
  BOOL              enough_mp;
  Transform*        transform;

  ecs_raw(g_ecs, CASTABLE, &entities, (pointer_t*)&castables, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (castables[i].timer > 0)
    {
      --castables[i].timer;
      // just finish
      if (castables[i].timer == 0)
      {
        if ((weapon_core = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
            (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)))
        {
          wearer_controller->in_action = FALSE;
        }
      }
    }

    if (castables[i].timer == 0 && (weapon_core = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
        (wearer_controller = ecs_get(g_ecs, weapon_core->wearer, CONTROLLER)) &&
        (attunement_slot = ecs_get(g_ecs, weapon_core->wearer, ATTUNEMENT_SLOT)) &&
        (mana_pool = ecs_get(g_ecs, weapon_core->wearer, MANA_POOL)) &&
        (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      spell     = &g_spell_tbl[attunement_slot->spell_id];
      trigger   = wearer_controller->action == CHARACTER_ACTION_REGULAR_ATK;
      enough_mp = mana_pool->mana_points >= spell->cost;
      if (trigger && enough_mp)
      {
        wearer_controller->action    = CHARACTER_ACTION_NONE;
        wearer_controller->in_action = TRUE;
        castables[i].timer           = spell->cast_spd;
        mana_pool->mana_points -= spell->cost;
        spell->cast_fn(g_ecs, weapon_core->wearer, entities[i]);
        g_cast_effect_fn_tbl[spell->casting_effect](g_ecs, transform->position);
      }
    }
  }
}
