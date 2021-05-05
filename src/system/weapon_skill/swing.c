#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;

static HandAnimKeyFrame _kframe[] = {
  { .duration = 5, .angle = -60.0 },
  { .duration = 7, .angle = 60.0 },
  { .duration = -1 },
};

struct __callback_data
{
  ecs_entity_t entity;
  int          damage;
  Vec2         impact_force;
};

static void cbquery(struct __callback_data* d, ecs_entity_t e)
{
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .dealer      = d->entity,
                    .receiver    = e,
                    .damage      = d->damage,
                    .type        = DAMAGE_TYPE_STRIKE,
                    .impact_time = 10,
                    .force       = d->impact_force,
                });
}

static void on_hand_anim_finished(SDL_UNUSED void* arg, ecs_entity_t entity)
{
  Controller*      ctl              = ecs_get(g_ecs, entity, CONTROLLER);
  HandAnimation*   hanim            = ecs_get(g_ecs, entity, HAND_ANIMATION);
  Hand*            hand             = ecs_get(g_ecs, entity, HAND);
  AttackMask*      mask             = ecs_get(g_ecs, entity, ATTACK_MASK);
  Transform*       transform        = ecs_get(g_ecs, entity, TRANSFORM);
  FacingDirection* facing_direction = ecs_get(g_ecs, entity, FACING_DIRECTION);

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

  struct __callback_data cbdata = { entity, wattrs->atk, vec2_mul(facing_direction->value, 60.f) };
  collision_box_query(&box, mask->value, CALLBACK_1(&cbdata, cbquery));
}

static void play_effect(Ecs* registry, ecs_entity_t entity, float length)
{
  Vec2             pos;
  double           rot;
  SDL_RendererFlip flip;
  Transform*       transform;
  Hand*            hand;
  FacingDirection* facing_direction;

  transform        = ecs_get(registry, entity, TRANSFORM);
  hand             = ecs_get(registry, entity, HAND);
  facing_direction = ecs_get(registry, entity, FACING_DIRECTION);

  pos = transform->position;
  pos = vec2_add(pos, hand->original_point);
  pos = vec2_add(pos, vec2_mul(facing_direction->value, length));

  rot = SDL_atan2f(facing_direction->value.y, facing_direction->value.x) * RAD_TO_DEG;
  rot += signf(facing_direction->value.x) * 45.0;

  flip = facing_direction->value.x > 0.f ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;

  make_fx_slash(registry, pos, rot, flip);
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
      ett_animate_hand(g_ecs,
                       holder->value,
                       _kframe,
                       TRUE,
                       CALLBACK_2(on_hand_anim_finished),
                       (Callback){ 0 });
      hctl->in_action = TRUE;
      hctl->action    = CHARACTER_ACTION_NONE;
      play_effect(g_ecs, holder->value, 16);
      Mix_PlayChannel(-1, get_sfx(SFX_WEAPON_SWORD), 0);
    }
  }
}
