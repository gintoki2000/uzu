#include "entity_factory.h"
#include "types.h"
#include "utils.h"

// <spell name>_cast

static void fire_ball_cast(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);
static void ice_arrow_cast(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);

const Spell g_spell_tbl[NUM_SPELLS] = {
  { "ice arrow", ice_arrow_cast, 1, 100, CAST_EFFECT_ICE },
  { "fire ball", fire_ball_cast, 1, 10, CAST_EFFECT_FIRE },
};

ecs_entity_t (*const g_cast_effect_fn_tbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos) = {
  [CAST_EFFECT_ICE]  = NULL,
  [CAST_EFFECT_FIRE] = NULL,
};

static void fire_ball_cast(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
}

static void ice_arrow_cast(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;

  Vec2 weapon_position = get_entity_position(ecs, weapon);
  Vec2 direction       = { 1.f, 0.f };

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_ice_arrow(ecs, weapon_position, direction, attributes->mask);  
}