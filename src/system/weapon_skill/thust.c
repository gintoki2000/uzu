#include "resources.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include <components.h>
#include <ecs/ecs.h>

extern Ecs* g_ecs;

typedef struct CBThust_QueryHitEntitiesArgs
{
  ecs_entity_t dealer;
  int          damage;
  float        direction;
} CBThust_QueryHitEntitiesArgs;

static const Vec2 k_impact_force = { 100.f, 0.f };

static BOOL __cb_query_hit_entities(CBThust_QueryHitEntitiesArgs* args, ecs_entity_t e)
{
  Vec2 force = vec2_mul(k_impact_force, args->direction);
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .dealer      = args->dealer,
                    .receiver    = e,
                    .damage      = args->damage,
                    .impact      = TRUE,
                    .force       = force,
                    .impact_time = 15,
                    .type        = DAMAGE_TYPE_THUST,
                });
  return TRUE;
}

void weapon_thust_attack_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponAttributes*  attributes;
  Holder*            holder;
  WeaponThustAttack* thust_attack;
  Controller*        holder_controller;
  Equipment*         holder_equipment;
  AttackMask*        holder_attack_mask;
  Motion*            holder_motion;
  FacingDirection*   holder_facing_direction;
  Transform*         transform;
  RECT               damage_area;

  ecs_raw(g_ecs, WEAPON_THUST_ATTACK, &entities, (pointer_t*)&thust_attack, &cnt);
  for (ecs_size_t i = 0; i < cnt; ++i)
  {
    if ((holder = ecs_get(g_ecs, entities[i], HOLDER)))
      switch (thust_attack[i].state)
      {
      case 0: // INACTIVE
        if ((holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
        {
          if (holder_controller->action == thust_attack[i].on_action)
          {
            holder_equipment        = ecs_get(g_ecs, holder->value, EQUIPMENT);
            holder_motion           = ecs_get(g_ecs, holder->value, MOTION);
            holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);
            transform               = ecs_get(g_ecs, entities[i], TRANSFORM);

            holder_equipment->adjustment.x  = -10;
            holder_equipment->adjustment.y  = -5;
            holder_controller->in_action    = TRUE;
            holder_controller->action       = CHARACTER_ACTION_NONE;
            holder_facing_direction->frezze = TRUE;

            transform->rotation = -15.0 * signf(holder_facing_direction->value.x);

            thust_attack[i].state = 1;
            thust_attack[i].timer = 7;
          }
        }
        break;
      case 1:
        if (TIMER_TICK(thust_attack[i].timer))
        {
          holder_equipment        = ecs_get(g_ecs, holder->value, EQUIPMENT);
          holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);
          transform               = ecs_get(g_ecs, entities[i], TRANSFORM);

          holder_equipment->adjustment.x = -8;
          holder_equipment->adjustment.y = -4;
          transform->rotation            = -12 * signf(holder_facing_direction->value.x);
          thust_attack[i].state          = 2;
          thust_attack[i].timer          = 2;
        }
        break;
      case 2:
        if (TIMER_TICK(thust_attack[i].timer))
        {
          holder_equipment = ecs_get(g_ecs, holder->value, EQUIPMENT);
          transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

          holder_equipment->adjustment.x = 10;
          holder_equipment->adjustment.y = 0;
          transform->rotation            = 0.0;
          thust_attack[i].state          = 3;
          thust_attack[i].timer          = 7;
          Mix_PlayChannel(-1, get_sfx(SFX_PUNCH), 0);
        }
        break;
      case 3:
        if ((holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
        {
          if (TIMER_TICK(thust_attack[i].timer))
          {
            holder_equipment        = ecs_get(g_ecs, holder->value, EQUIPMENT);
            transform               = ecs_get(g_ecs, entities[i], TRANSFORM);
            attributes              = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES);
            holder_attack_mask      = ecs_get(g_ecs, holder->value, ATTACK_MASK);
            holder_facing_direction = ecs_get(g_ecs, holder->value, FACING_DIRECTION);

            holder_controller->in_action    = FALSE;
            holder_equipment->adjustment.x  = 0;
            holder_equipment->adjustment.y  = 0;
            thust_attack[i].state           = 0;
            holder_facing_direction->frezze = FALSE;

            damage_area.x = transform->position.x - 20;
            damage_area.y = transform->position.y - 9;
            damage_area.w = 40;
            damage_area.h = 12;

            collision_box_query(&damage_area,
                                attributes->atk,
                                CALLBACK_1((&(CBThust_QueryHitEntitiesArgs){
                                               .dealer    = holder->value,
                                               .damage    = attributes->atk,
                                               .direction = signf(holder_facing_direction->value.x),
                                           }),
                                           __cb_query_hit_entities));
          }
        }
      }
  }
}
