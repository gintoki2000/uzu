#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;

static HandAnimKeyFrame _kframe[] = {
  { .duration = 5, .angle = -60.0 },
  { .duration = 5, .angle = 60.0 },
  { .duration = -1 },
};

struct __callback_data
{
  ecs_entity_t entity;
  int          damage;
};

static void cbquery(struct __callback_data* d, ecs_entity_t e)
{
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .dealer      = d->entity,
                    .receiver    = e,
                    .damage      = d->damage,
                    .type        = DAMAGE_TYPE_STRIKE,
                    .impact      = FALSE,
                    .impact_time = 1,
                });
}

static void on_hand_anim_finished(SDL_UNUSED void* arg, ecs_entity_t entity)
{
  Controller*    ctl       = ecs_get(g_ecs, entity, CONTROLLER);
  HandAnimation* hanim     = ecs_get(g_ecs, entity, HAND_ANIMATION);
  Hand*          hand      = ecs_get(g_ecs, entity, HAND);
  AttackMask*    mask      = ecs_get(g_ecs, entity, ATTACK_MASK);
  Transform*     transform = ecs_get(g_ecs, entity, TRANSFORM);

  WeaponAttributes*  wattrs = ecs_get(g_ecs, hand->weapon, WEAPON_ATTRIBUTES);
  WeaponSwingAttack* swing  = ecs_get(g_ecs, hand->weapon, WEAPON_SWING_ATTACK);

  ctl->in_action = FALSE;
  double a       = hanim->initial_angle * DEG_TO_RAD;
  Vec2   v;
  v.x = SDL_cos(a) * wattrs->range;
  v.y = SDL_sin(a) * wattrs->range;
  v   = vec2_add(v, transform->position);
  v   = vec2_add(v, hand->original_point);

  RECT box;
  box.x = v.x - swing->wide / 2;
  box.y = v.y - swing->wide / 2;
  box.w = box.h = swing->wide;

  struct __callback_data cbdata = { entity, wattrs->atk };
  collision_box_query(&box, mask->value, CALLBACK_1(&cbdata, cbquery));
}

void weapon_swing_attack_system(void)
{
  ecs_entity_t*      entities;
  ecs_size_t         cnt;
  WeaponSwingAttack* c;
  Holder*            holder;
  Controller*        hctl;

  ecs_raw(g_ecs, WEAPON_SWING_ATTACK, &entities, (void**)&c, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    holder = ecs_get(g_ecs, entities[i], HOLDER);
    if (!ecs_is_valid(g_ecs, holder->value))
      continue;
    hctl = ecs_get(g_ecs, holder->value, CONTROLLER);
    if (!hctl->in_action && hctl->action == c->on_action &&
        !ecs_has(g_ecs, holder->value, HAND_ANIMATION))
    {
      animate_entity_hand(g_ecs,
                          holder->value,
                          _kframe,
                          TRUE,
                          CALLBACK_2(on_hand_anim_finished),
                          (Callback){ 0 });
      hctl->in_action = TRUE;
      hctl->action    = CHARACTER_ACTION_NONE;
    }
  }
}
