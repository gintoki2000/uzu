#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"

static void fire_ball_cast(ecs_Registry* registry, ecs_entity_t caster, ecs_entity_t weapon);
static void ice_arrow_cast(ecs_Registry* registry, ecs_entity_t caster, ecs_entity_t weapon);

static BOOL dummy_process_callback(ecs_Registry*, ecs_entity_t, ecs_entity_t);
static void dummy_cast_callback(ecs_Registry*, ecs_entity_t, ecs_entity_t);

static void* get_spell_state(ecs_Registry* registry, ecs_entity_t weapon)
{
  WeaponCast* component = ecs_get(registry, weapon, WEAPON_CAST);
  return (void*)component->state;
}
#define SPELL_ICON(n) { TEX_ICON_##n, { 0, 0, 16, 16} }
const Spell gSpellTbl[NUM_SPELLS] = {
  { "ice arrow", ice_arrow_cast, dummy_process_callback, 1, 15, SPELL_ICON(ICE_ARROW) },
  { "fire ball", fire_ball_cast, dummy_process_callback, 1, 10, SPELL_ICON(FIRE_BALL) },
};

#define PLAY_SOUND(id) Mix_PlayChannel(-1, get_sfx(id), 0)


/*
static Vec2 vec2_rot(Vec2 v, float angle)
{
  float s = SDL_sinf(angle);
  float c = SDL_cosf(angle);
  Vec2  o;
  o.x = c * v.x - s * v.y;
  o.y = s * v.x + c * v.y;
  return o;
}
*/

static void fire_ball_cast(ecs_Registry* registry, ecs_entity_t caster, SDL_UNUSED ecs_entity_t weapon)
{
  const float SPEED = 250.f;
  Vec2  position;
  u16   attackMask;
  Vec2  aimDirection;
  Vec2  projectileSpeed; 

  position         = ett_get_position        (registry, caster);
  attackMask       = ett_get_atk_mask        (registry, caster);
  aimDirection     = ett_get_facing_direction(registry, caster);

  projectileSpeed = vec2_mul(aimDirection, SPEED);
  make_fire_ball(registry, caster, position, projectileSpeed, attackMask);
  PLAY_SOUND(SFX_FIRE_BALL_LAUCH);
}

static void ice_arrow_cast(ecs_Registry* registry, ecs_entity_t caster, ecs_entity_t SDL_UNUSED weapon)
{
  const float SPEED = 250.f;
  Vec2  position;
  u16   attackMask;
  Vec2  aimDirection;
  Vec2  projectileSpeed;

  position         = ett_get_position(registry, caster);
  attackMask       = ett_get_atk_mask(registry, caster);
  aimDirection     = ett_get_facing_direction(registry, caster);

  projectileSpeed = vec2_mul(aimDirection, SPEED);

  make_ice_arrow(registry, caster, position, projectileSpeed, attackMask);

  PLAY_SOUND(SFX_ICE_SHOOT);
}


static BOOL dummy_process_callback(SDL_UNUSED ecs_Registry*         registry, 
                                   SDL_UNUSED ecs_entity_t caster, 
                                   SDL_UNUSED ecs_entity_t weapon)
{
  return TRUE;
}

static void dummy_cast_callback(SDL_UNUSED ecs_Registry*         registry, 
                                SDL_UNUSED ecs_entity_t caster, 
                                SDL_UNUSED ecs_entity_t weapon)
{
}
