#include "components.h"
#include "entity_factory.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
extern Ecs* g_ecs;

typedef struct CBSwing_QueryHitEntitiesArgs
{
  ecs_entity_t dealer;
  int          damage;
  Vec2         force;
} CBSwing_QueryHitEntitiesArgs;

static const Vec2 _impact_force = { 50.f, 0.f };

static BOOL __cb_query_hit_entities(CBSwing_QueryHitEntitiesArgs* args, ecs_entity_t e)
{

  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){ .dealer      = args->dealer,
                                   .receiver    = e,
                                   .damage      = args->damage,
                                   .impact      = TRUE,
                                   .impact_time = 10,
                                   .force       = args->force });
  return TRUE;
}

#define ATTACK_AREA_HEIGHT (32)

INLINE void get_damage_area(RECT* out, Vec2 position, int range, int horizontal_axis)
{
  out->w = range;
  out->h = ATTACK_AREA_HEIGHT;
  out->y = position.y - 16;
  out->x = horizontal_axis ? position.x : position.x - range;
}

void weapon_swing_attack_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponSwingAttack* swing_attack;
  WeaponAttributes*  attributes;
  Transform*         transform;
  Controller*        holder_controller;
  Holder*            holder;
  AttackMask*        holder_attack_mask;
  FacingDirection*   holder_facing_direction;

  RECT damage_area;

  ecs_raw(g_ecs, WEAPON_SWING_ATTACK, &entities, (void**)&swing_attack, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    switch (swing_attack[i].state)
    {
    case 0:
      if ((holder = ecs_get(g_ecs, entities[i], HOLDER)) &&
          (holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
      {
        if (!holder_controller->in_action && holder_controller->action == swing_attack[i].on_action)
        {
          transform               = ecs_get(g_ecs, entities[i], TRANSFORM);
          holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);

          swing_attack[i].timer           = 5;
          swing_attack[i].state           = 1;
          holder_controller->in_action    = TRUE;
          holder_controller->action       = CHARACTER_ACTION_NONE;
          transform->rotation             = -15 * signf(holder_facing_direction->value.x);
          holder_facing_direction->frezze = TRUE;
        }
      }
      break;
    case 1:
      if (swing_attack[i].timer > 0 && --swing_attack[i].timer == 0)
      {
        transform               = ecs_get(g_ecs, entities[i], TRANSFORM);
        holder                  = ecs_get(g_ecs, entities[i], HOLDER);
        holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);

        float horizontal_axis = signf(holder_facing_direction->value.x);

        swing_attack[i].state = 2;
        swing_attack[i].timer = 7;
        transform->rotation   = 115 * horizontal_axis;

        SDL_RendererFlip flip = horizontal_axis < 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

        Vec2 particle_position;
        particle_position.x = transform->position.x + swing_attack[i].range / 2.f * horizontal_axis;
        particle_position.y = transform->position.y - 13;
        make_fx_slash(g_ecs, particle_position, flip);
        Mix_PlayChannel(-1, get_sfx(SFX_WEAPON_SWORD), 0);
      }
      break;
    case 2:
      if (swing_attack[i].timer > 0 && --swing_attack[i].timer == 0)
      {
        attributes              = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES);
        transform               = ecs_get(g_ecs, entities[i], TRANSFORM);
        holder                  = ecs_get(g_ecs, entities[i], HOLDER);
        holder_controller       = ecs_get(g_ecs, holder->value, CONTROLLER);
        holder_attack_mask      = ecs_get(g_ecs, holder->value, ATTACK_MASK);
        holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);

        transform->rotation             = 0.0;
        swing_attack[i].state           = 0;
        holder_controller->in_action    = FALSE;
        holder_facing_direction->frezze = FALSE;

        float horizontal_axis = signf(holder_facing_direction->value.x);

        get_damage_area(&damage_area, transform->position, swing_attack[i].range, 1);
        collision_box_query(&damage_area,
                            holder_attack_mask->value,
                            CALLBACK_1((&(CBSwing_QueryHitEntitiesArgs){
                                           .damage = attributes->atk,
                                           .force  = vec2_mul(_impact_force, horizontal_axis),
                                           .dealer = holder->value,
                                       }),
                                       __cb_query_hit_entities));
      }
    }
  }
}
