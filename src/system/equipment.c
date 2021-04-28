#include "components.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void weapon_transform_system()
{
  ecs_entity_t* entities;
  ecs_size_t    count;
  Hand*         hand;
  // owner components
  Transform*       transform;
  FacingDirection* fdir;

  // weapon components
  WeaponAttributes* weapon_attributes;
  Transform*        weapon_transform;
  Visual*           weapon_visual;

  Vec2 weapon_position;
  Vec2 attachment_point;

  double s, c, a;
  ecs_raw(g_ecs, HAND, &entities, (void**)&hand, &count);
  for (int i = 0; i < count; ++i)
  {
    if (hand[i].weapon == ECS_NULL_ENT)
      continue;
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);
    fdir      = ecs_get(g_ecs, entities[i], FACING_DIRECTION);

    weapon_attributes = ecs_get(g_ecs, hand[i].weapon, WEAPON_ATTRIBUTES);
    weapon_transform  = ecs_get(g_ecs, hand[i].weapon, TRANSFORM);
    weapon_visual     = ecs_get(g_ecs, hand[i].weapon, VISUAL);

    weapon_position  = vec2_add(transform->position, hand->original_point);
    a                = hand[i].angle * DEG_TO_RAD;
    s                = SDL_sin(a);
    c                = SDL_cos(a);
    attachment_point = vec2_mul((Vec2){ c, s }, hand[i].length);

    weapon_position = vec2_add(weapon_position, attachment_point);

    weapon_transform->position = weapon_position;
    weapon_transform->rotation = hand[i].angle + signf(c) * weapon_attributes->base_angle;
    weapon_visual->flip        = fdir->value.x > 0.f ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
  }
}

static BOOL next_kframe(HandAnimation* anim)
{
  ++anim->current_index;
  if (anim->keyframes[anim->current_index].duration == -1)
    return FALSE;
  anim->current_duration = anim->keyframes[anim->current_index].duration;
  return TRUE;
}

void hand_animation_system(void)
{
  ecs_entity_t*           entities;
  ecs_size_t              cnt;
  HandAnimation*          hand_anim;
  Hand*                   hand;
  const HandAnimKeyFrame* kf;
  FacingDirection*        fdir;

  BOOL not_started;
  BOOL just_finished_kframe;
  ecs_raw(g_ecs, HAND_ANIMATION, &entities, (void**)&hand_anim, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    not_started          = hand_anim[i].current_index == -1;
    just_finished_kframe = hand_anim[i].current_duration > 0 && !(--hand_anim[i].current_duration);
    if (not_started || just_finished_kframe)
    {
      if (next_kframe(&hand_anim[i]))
      {
        hand = ecs_get(g_ecs, entities[i], HAND);
        fdir = ecs_get(g_ecs, entities[i], FACING_DIRECTION);

        INVOKE_EVENT(hand_anim[i].frame_callback, entities[i], hand_anim[i].current_index);
        kf                   = hand_anim[i].keyframes + hand_anim[i].current_index;
        hand->angle          = hand_anim[i].initial_angle + kf->angle * signf(fdir->value.x);
        hand->length         = hand_anim[i].initial_length + kf->length;
        hand->original_point = vec2_add(hand_anim[i].initial_point, kf->original);
      }
      else
      {
        INVOKE_EVENT(hand_anim[i].finished_callback, entities[i]);
        ecs_rmv(g_ecs, entities[i], HAND_ANIMATION);
      }
    }
  }
}

void hand_system(void)
{
  ecs_entity_t*     entities;
  ecs_size_t        cnt;
  Hand*             hand;
  FacingDirection*  fdir;
  WeaponAttributes* attrs;

  ecs_raw(g_ecs, HAND, &entities, (void**)&hand, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (ecs_has(g_ecs, entities[i], HAND_ANIMATION))
      continue;
    fdir          = ecs_get(g_ecs, entities[i], FACING_DIRECTION);
    attrs         = ecs_get(g_ecs, hand[i].weapon, WEAPON_ATTRIBUTES);
    hand[i].angle = SDL_atan2f(fdir->value.y, fdir->value.x) * 57.2957795;
  }
}

void equipment_system()
{
  hand_system();
  hand_animation_system();
  weapon_transform_system();
}
