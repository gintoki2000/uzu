#include "components.h"
#include "ecs/ecs.h"
#include "global.h"

extern const Spell g_spell_tbl[];
extern Ecs*        g_ecs;

void weapon_casting_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Holder*           holder;
  Castable*         castables;
  WeaponAttributes* attributes;
  Controller*       holder_controller;
  AttunementSlot*   attunement_slot;
  ManaPool*         mana_pool;
  const Spell*      spell;
  BOOL              trigger;
  BOOL              enough_mp;
  Transform*        transform;

  ecs_raw(g_ecs, WEAPON_CAST, &entities, (pointer_t*)&castables, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (castables[i].timer > 0)
    {
      --castables[i].timer;
      // just finish
      if (castables[i].timer == 0)
      {
        if ((holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
            (holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
        {
          holder_controller->in_action = FALSE;
        }
      }
    }

    if (castables[i].timer == 0 && (attributes = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES)) &&
        (holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
        (holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)) &&
        (attunement_slot = ecs_get(g_ecs, holder->value, ATTUNEMENT_SLOT)) &&
        (mana_pool = ecs_get(g_ecs, holder->value, MANA_POOL)) &&
        (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      if (attunement_slot->spell_id == SPELL_ID_NULL)
        continue;
      spell     = &g_spell_tbl[attunement_slot->spell_id];
      trigger   = holder_controller->action == CHARACTER_ACTION_REGULAR_ATK;
      enough_mp = mana_pool->mana_points >= spell->cost;
      if (trigger && enough_mp)
      {
        holder_controller->action    = CHARACTER_ACTION_NONE;
        holder_controller->in_action = TRUE;
        castables[i].timer           = spell->cast_spd;
        mana_pool->mana_points -= spell->cost;
        spell->cast(g_ecs, holder->value, entities[i]);
        g_make_cast_effect_fn_tbl[spell->casting_effect](g_ecs, transform->position);
      }
    }
  }
}
