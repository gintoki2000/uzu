#include "entity_factory.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"

static void fire_ball_cast(Ecs* registry, ecs_entity_t caster, ecs_entity_t weapon);
static void ice_arrow_cast(Ecs* registry, ecs_entity_t caster, ecs_entity_t weapon);

static BOOL default_process_func(Ecs* registry, ecs_entity_t caster, ecs_entity_t weapon);

struct __HomieFire
{
    int remaining;
    int cooldown;
};

static void* get_spell_state(Ecs* registry, ecs_entity_t weapon)
{
  WeaponCast* component = ecs_get(registry, weapon, WEAPON_CAST);
  return (void*)component->state;
}
#define SPELL_ICON(n) { TEX_ICON_##n, { 0, 0, 16, 16} }
const Spell gSpellTbl[NUM_SPELLS] = {
  /*Name               CastFunc              ProcFunc              Cost  Cooldown  Icon */
  { "ice arrow", ice_arrow_cast, default_process_func, 1, 15, SPELL_ICON(ICE_ARROW) },
  { "fire ball", fire_ball_cast, default_process_func, 1, 10, SPELL_ICON(FIRE_BALL) },
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

static void fire_ball_cast(Ecs* registry, ecs_entity_t caster, SDL_UNUSED ecs_entity_t weapon)
{
  Vec2  position;
  u16   attack_mask;
  Vec2  facing_direction;
  Vec2  projectile_speed; 

  position         = ett_get_position(registry, caster);
  attack_mask      = ett_get_atk_mask(registry, caster);
  facing_direction = ett_get_facing_direction(registry, caster);

  projectile_speed = vec2_mul(facing_direction, 250.f);
  make_fire_ball(registry, caster, position, projectile_speed, attack_mask);
  PLAY_SOUND(SFX_FIRE_BALL_LAUCH);
}

static void ice_arrow_cast(Ecs* registry, ecs_entity_t caster, ecs_entity_t SDL_UNUSED weapon)
{
  Vec2  position;
  u16   attack_mask;
  Vec2  facing_direction;
  Vec2  projectile_speed; 

  position         = ett_get_position(registry, caster);
  attack_mask      = ett_get_atk_mask(registry, caster);
  facing_direction = ett_get_facing_direction(registry, caster);

  projectile_speed = vec2_mul(facing_direction, 250.f);

  make_ice_arrow(registry, caster, position, projectile_speed, attack_mask);

  PLAY_SOUND(SFX_ICE_SHOOT);
}


static BOOL default_process_func(SDL_UNUSED Ecs*         registry, 
                                 SDL_UNUSED ecs_entity_t caster  , 
                                 SDL_UNUSED ecs_entity_t weapon  )
{
  return TRUE;
}
