#include "components.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "global.h"

extern const Spell g_spell_tbl[];
extern Ecs*        g_ecs;

const HandAnimKeyFrame _key_frames[] = {
  { .duration = 2, .length = -1, .angle = -15.0 },
  { .duration = 2, .length = -2, .angle = -20.0 },
  { .duration = 3, .length = 3, .angle = 0 },
  { .duration = -1 },
};

void weapon_casting_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponCast*  cast;
  Controller*  controller;
  Mana*        mana;
  const Spell* spell;
  BOOL         is_trigger;
  BOOL         has_enough_mp;
  ecs_entity_t holder;

  ecs_raw(g_ecs, WEAPON_CAST, &entities, (pointer_t*)&cast, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (cast[i].processing)
    {
      holder     = wpn_get_holder(g_ecs, entities[i]);
      spell      = ett_get_attuned_spell(g_ecs, holder);
      controller = ecs_get(g_ecs, holder, CONTROLLER);
      if (spell->process(g_ecs, holder, entities[i]))
      {
        cast->processing      = FALSE;
        controller->in_action = FALSE;
      }
    }
    else if (cast[i].cooldown_timer == 0 &&
             (holder = wpn_get_holder(g_ecs, entities[i])) != ECS_NULL_ENT &&
             (spell = ett_get_attuned_spell(g_ecs, holder)) &&
             (controller = ecs_get(g_ecs, holder, CONTROLLER)) &&
             (mana = ecs_get(g_ecs, holder, MANA)))
    {
      is_trigger    = !controller->in_action && controller->action == CHARACTER_ACTION_REGULAR_ATK;
      has_enough_mp = mana->current >= spell->cost;

      if (is_trigger)
      {
        controller->action = CHARACTER_ACTION_NONE;
        if (has_enough_mp)
        {
          mana->current -= spell->cost;
          spell->cast(g_ecs, holder, entities[i]);
          cast[i].processing     = TRUE;
          cast[i].cooldown_timer = spell->cast_spd;
          controller->in_action  = TRUE;
          ett_animate_hand(g_ecs, holder, _key_frames, TRUE, (Callback){ 0 }, (Callback){ 0 });
        }
      }
    }
    else
    {
      --cast[i].cooldown_timer;
    }
  }
}
