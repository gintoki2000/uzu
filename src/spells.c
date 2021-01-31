#include "entity_factory.h"
#include "resources.h"
#include "types.h"
#include "utils.h"

static void cast_fire_ball(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);
static void cast_ice_arrow(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);

const Spell g_spell_tbl[NUM_SPELLS] = {
  { "ice arrow", cast_ice_arrow, 1, 15, CAST_EFFECT_ICE },
  { "fire ball", cast_fire_ball, 1, 10, CAST_EFFECT_FIRE },
};

ecs_entity_t (*const g_cast_effect_fn_tbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos) = {
  [CAST_EFFECT_ICE]  = make_ice_cast_effect,
  [CAST_EFFECT_FIRE] = make_fire_cast_effect,
};

static void cast_fire_ball(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;

  transform = ecs_get(ecs, weapon, TRANSFORM);

  Vec2 speed = { 100.f * transform->hdir, 0.f };

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_fire_ball(ecs, transform->position, speed, attributes->mask);

  Mix_PlayChannel(-1, get_sfx(SFX_ICE_SHOOT), 0);
}

static void cast_ice_arrow(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;

  transform = ecs_get(ecs, weapon, TRANSFORM);

  Vec2 speed = { 100.f * transform->hdir, 0.f };

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_ice_arrow(ecs, transform->position, speed, attributes->mask);

  Mix_PlayChannel(-1, get_sfx(SFX_ICE_SHOOT), 0);
}
