#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"

static void cast_fire_ball(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);
static void cast_ice_arrow(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);

#define ICON_ICE_ARROW                                                                             \
  {                                                                                                \
    TEX_ICON_ICE_ARROW,                                                                            \
    {                                                                                              \
      0, 0, 16, 16                                                                                 \
    }                                                                                              \
  }
#define ICON_FIRE_BALL                                                                             \
  {                                                                                                \
    TEX_ICON_FIRE_BALL,                                                                            \
    {                                                                                              \
      0, 0, 16, 16                                                                                 \
    }                                                                                              \
  }

const Spell g_spell_tbl[NUM_SPELLS] = {
  { "ice arrow", cast_ice_arrow, 1, 15, CAST_EFFECT_ICE, ICON_ICE_ARROW },
  { "fire ball", cast_fire_ball, 1, 10, CAST_EFFECT_FIRE, ICON_FIRE_BALL },
};

ecs_entity_t (*const g_make_cast_effect_fn_tbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos) = {
  [CAST_EFFECT_ICE]  = make_fx_cast_ice,
  [CAST_EFFECT_FIRE] = make_fx_cast_fire,
};

#define play_sound(__sound_id) Mix_PlayChannel(-1, get_sfx(__sound_id), 0)

static Vec2 vec2_rot(Vec2 v, float angle)
{
  float s = SDL_sinf(angle);
  float c = SDL_cosf(angle);
  Vec2  o;
  o.x = c * v.x - s * v.y;
  o.y = s * v.x + c * v.y;
  return o;
}

static void cast_fire_ball(Ecs* ecs, SDL_UNUSED ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;
  AttackMask*       attack_mask;
  FacingDirection*  facing_direction;

  transform        = ecs_get(ecs, weapon, TRANSFORM);
  attack_mask      = ecs_get(ecs, caster, ATTACK_MASK);
  facing_direction = ecs_get(ecs, caster, FACING_DIRECTION);

  Vec2 speed1 = vec2_mul(facing_direction->value, 120.f);
  Vec2 speed2 = vec2_rot(speed1, -0.5235987756f);
  Vec2 speed3 = vec2_rot(speed1, 0.5235987756f);

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_fire_ball(ecs, caster, transform->position, speed1, attack_mask->value);
  make_fire_ball(ecs, caster, transform->position, speed2, attack_mask->value);
  make_fire_ball(ecs, caster, transform->position, speed3, attack_mask->value);
  play_sound(SFX_FIRE_BALL_LAUCH);
}

static void cast_ice_arrow(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon)
{
  WeaponAttributes* attributes;
  Transform*        transform;
  AttackMask*       attack_mask;
  FacingDirection*  facing_direction;

  transform        = ecs_get(ecs, weapon, TRANSFORM);
  attack_mask      = ecs_get(ecs, caster, ATTACK_MASK);
  facing_direction = ecs_get(ecs, caster, FACING_DIRECTION);

  Vec2 speed = vec2_mul(facing_direction->value, 120.f);

  attributes = ecs_get(ecs, weapon, WEAPON_ATTRIBUTES);

  make_ice_arrow(ecs, caster, transform->position, speed, attack_mask->value);

  play_sound(SFX_ICE_SHOOT);
}

#undef play_sound
