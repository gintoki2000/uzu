#include "thust.h"
#include "../collision_sys.h"
#include "../event_messaging_sys.h"
#include <components.h>
#include <ecs/ecs.h>

extern Ecs* g_ecs;

typedef struct CBThust_QueryHitEntitiesArgs
{
  ecs_entity_t dealer;
  int          damage;
  int          direction; // -1 / 1
} CBThust_QueryHitEntitiesArgs;

static const Vec2 IMPACT_FORCE = { 100.f, 0.f };

static BOOL __cb_query_hit_entities(CBThust_QueryHitEntitiesArgs* args, ecs_entity_t e)
{
  Vec2 force = vec2_mul(IMPACT_FORCE, args->direction);
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .dealer      = args->dealer,
                    .receiver    = e,
                    .damage      = args->damage,
                    .impact      = TRUE,
                    .force       = force,
                    .impact_time = 10,
                    .type        = DAMAGE_TYPE_THUST,
                });
  return TRUE;
}

#define TIMER_TICK(__timer) ((__timer) > 0 && --(__timer) == 0)

void weapon_skill_thust_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  WeaponAttributes* attributes;
  Holder*           holder;
  wpskl_Thust*      skl;
  Controller*       holder_controller;
  Equipment*        holder_equipment;
  Motion*           holder_motion;
  Transform*        transform;
  RECT              damage_area;

  ecs_raw(g_ecs, WEAPON_SKILL_THUST, &entities, (pointer_t*)&skl, &cnt);
  for (ecs_size_t i = 0; i < cnt; ++i)
  {
    if ((holder = ecs_get(g_ecs, entities[i], HOLDER)))
      switch (skl[i].state)
      {
      case 0: // INACTIVE
        if ((holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
        {
          if (holder_controller->action == skl[i].on_action)
          {
            holder_equipment = ecs_get(g_ecs, holder->value, EQUIPMENT);
            holder_motion    = ecs_get(g_ecs, holder->value, MOTION);
            transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

            holder_equipment->d.x        = -10;
            holder_equipment->d.y        = -5;
            holder_controller->in_action = TRUE;
            holder_controller->action    = CHARACTER_ACTION_NONE;

            transform->rotation = -15.0 * transform->hdir;

            skl[i].state = 1;
            skl[i].timer = 7;
          }
        }
        break;
      case 1:
        if (TIMER_TICK(skl[i].timer))
        {
          holder_equipment = ecs_get(g_ecs, holder->value, EQUIPMENT);
          transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

          holder_equipment->d.x = -8;
          holder_equipment->d.y = -4;
          transform->rotation   = -12 * transform->hdir;
          skl[i].state          = 2;
          skl[i].timer          = 2;
        }
        break;
      case 2:
        if (TIMER_TICK(skl[i].timer))
        {
          holder_equipment = ecs_get(g_ecs, holder->value, EQUIPMENT);
          transform        = ecs_get(g_ecs, entities[i], TRANSFORM);

          holder_equipment->d.x = 10;
          holder_equipment->d.y = 0;
          transform->rotation   = 0.0;
          skl[i].state          = 3;
          skl[i].timer          = 7;
        }
        break;
      case 3:
        if ((holder_controller = ecs_get(g_ecs, holder->value, CONTROLLER)))
        {
          if (TIMER_TICK(skl[i].timer))
          {
            holder_equipment = ecs_get(g_ecs, holder->value, EQUIPMENT);
            transform        = ecs_get(g_ecs, entities[i], TRANSFORM);
            attributes       = ecs_get(g_ecs, entities[i], WEAPON_ATTRIBUTES);

            holder_controller->in_action = FALSE;
            holder_equipment->d.x        = 0;
            holder_equipment->d.y        = 0;
            skl[i].state                 = 0;

            damage_area.x = transform->position.x - 20;
            damage_area.y = transform->position.y - 9;
            damage_area.w = 40;
            damage_area.h = 12;

            collision_box_query(&damage_area,
                                attributes->mask,
                                CALLBACK_1((&(CBThust_QueryHitEntitiesArgs){
                                               .dealer    = holder->value,
                                               .damage    = attributes->atk,
                                               .direction = transform->hdir,
                                           }),
                                           __cb_query_hit_entities));
          }
        }
      }
  }
}

#undef TIMER_TICK
