#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;

static HandAnimKeyFrame _keyframes[] = {
  { .duration = 5, .angle = -60.0 },
  { .duration = 7, .angle = 60.0 },
  { .duration = -1 },
};

struct _query_overlap_callback_data
{
  ecs_entity_t entity;
  int          damage;
  Vec2         impact_force;
};

static void query_overlap_callback(struct _query_overlap_callback_data* d, ecs_entity_t e)
{
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(InflictDamageMsg){
                    .dealer      = d->entity,
                    .receiver    = e,
                    .damage      = d->damage,
                    .type        = DAMAGE_TYPE_STRIKE,
                    .impact_time = 10,
                    .force       = d->impact_force,
                });
}

static void hand_animation_finished_callback(SDL_UNUSED void* arg, ecs_entity_t entity)
{
  HandAnimation*   hanim           = ecs_get(g_ecs, entity, HAND_ANIMATION);
  Hand*            hand            = ecs_get(g_ecs, entity, HAND);
  AttackMask*      mask            = ecs_get(g_ecs, entity, ATTACK_MASK);
  Transform*       transform       = ecs_get(g_ecs, entity, TRANSFORM);
  FacingDirection* facingDirection = ecs_get(g_ecs, entity, FACING_DIRECTION);
  AttackCommand*   attackCommand   = ecs_get(g_ecs, entity, ATTACK_COMMAND);

  WeaponAttributes*  weaponAttributes = ecs_get(g_ecs, hand->weapon, WEAPON_ATTRIBUTES);
  WeaponSwingAttack* swing            = ecs_get(g_ecs, hand->weapon, WEAPON_SWING_ATTACK);

  INVOKE_EVENT(attackCommand->cbCompleted, TRUE);
  double a        = hanim->initialAngle * DEG_TO_RAD;
  int    midRange = (weaponAttributes->range.min + weaponAttributes->range.max) / 2;

  Vec2 v;
  v.x = SDL_cos(a) * midRange;
  v.y = SDL_sin(a) * midRange;
  v   = vec2_add(v, transform->position);
  v   = vec2_add(v, hand->originalPoint);

  RECT box;
  box.x = v.x - swing->wide / 2;
  box.y = v.y - swing->wide / 2;
  box.w = box.h = swing->wide;

  struct _query_overlap_callback_data cbdata = { entity,
                                                 weaponAttributes->atk,
                                                 vec2_mul(facingDirection->value, 60.f) };
  collision_box_query(&box, mask->value, CALLBACK_1(&cbdata, query_overlap_callback));
}

static void play_slash_effect(Ecs* registry, ecs_entity_t entity, float length)
{
  Vec2             pos;
  double           rot;
  SDL_RendererFlip flip;
  Transform*       transform;
  Hand*            hand;
  FacingDirection* facingDirection;

  transform       = ecs_get(registry, entity, TRANSFORM);
  hand            = ecs_get(registry, entity, HAND);
  facingDirection = ecs_get(registry, entity, FACING_DIRECTION);

  pos = transform->position;
  pos = vec2_add(pos, hand->originalPoint);
  pos = vec2_add(pos, vec2_mul(facingDirection->value, length));

  rot = SDL_atan2f(facingDirection->value.y, facingDirection->value.x) * RAD_TO_DEG;
  rot += signf(facingDirection->value.x) * 45.0;

  flip = facingDirection->value.x > 0.f ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;

  make_fx_slash(registry, pos, rot, flip);
}

void weapon_swing_attack_system(void)
{
  ecs_entity_t*      entities;
  ecs_size_t         cnt;
  WeaponSwingAttack* swing;
  Holder*            holder;
  AttackCommand*     attackCommand;

  ecs_raw(g_ecs, WEAPON_SWING_ATTACK, &entities, (void**)&swing, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    holder = ecs_get(g_ecs, entities[i], HOLDER);
    if (holder == NULL || !ecs_is_valid(g_ecs, holder->value))
      continue;

    attackCommand = ecs_get(g_ecs, holder->value, ATTACK_COMMAND);
    if (attackCommand == NULL || attackCommand->processing)
      continue;

    if (attackCommand->code == swing[i].code && !ecs_has(g_ecs, holder->value, HAND_ANIMATION))
    {
      HandAnimParams params       = { 0 };
      params.cbFinished           = CALLBACK_2(hand_animation_finished_callback);
      params.keyframes            = _keyframes;
      params.realtiveCurrentState = TRUE;
      ett_animate_hand(g_ecs, holder->value, &params);
      attackCommand->processing = TRUE;
      play_slash_effect(g_ecs, holder->value, 16);
      Mix_PlayChannel(-1, get_sfx(SFX_WEAPON_SWORD), 0);
    }
  }
}
