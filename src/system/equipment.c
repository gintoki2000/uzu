#include "components.h"
#include "config.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

void weapon_transform_system()
{
  ecs_entity_t* entities;
  ecs_size_t    count;
  Hand*         hand;
  // owner components
  Transform*       transform;
  AimDirection*    aimDirection;

  // weapon components
  Transform*        weaponTransform;
  Visual*           weaponVisual;

  Vec2 weaponPosition;
  Vec2 attachPoint;

  double s, c, a;
  ecs_raw(gEcs, HAND, &entities, (void**)&hand, &count);
  for (int i = 0; i < count; ++i)
  {
    if (hand[i].weapon == ECS_NULL_ENT)
      continue;
    transform    = ecs_get(gEcs, entities[i], TRANSFORM);
    aimDirection = ecs_get(gEcs, entities[i], AIM_DIRECTION);

    weaponTransform  = ecs_get(gEcs, hand[i].weapon, TRANSFORM);
    weaponVisual     = ecs_get(gEcs, hand[i].weapon, VISUAL);

    weaponPosition = vec2_add(transform->position, hand->originalPoint);
    a              = hand[i].angle * DEG_TO_RAD;
    s              = SDL_sin(a);
    c              = SDL_cos(a);
    attachPoint    = vec2_mul((Vec2){ c, s }, hand[i].length);

    weaponPosition = vec2_add(weaponPosition, attachPoint);

    weaponTransform->position = weaponPosition;
    weaponTransform->rotation = hand[i].angle;
    weaponVisual->flip        = aimDirection->value.x > 0.f ? SDL_FLIP_NONE : SDL_FLIP_VERTICAL;
  }
}

static BOOL next_kframe(HandAnimation* anim)
{
  ++anim->currentIndex;
  if (anim->keyframes[anim->currentIndex].duration == -1)
    return FALSE;
  anim->currentDuration = anim->keyframes[anim->currentIndex].duration;
  return TRUE;
}

void hand_animation_system(void)
{
  ecs_entity_t*           entities;
  ecs_size_t              cnt;
  HandAnimation*          handAnim;
  Hand*                   hand;
  const HandAnimKeyFrame* kf;
  AimDirection*           fdir;

  BOOL notStarted;
  BOOL justFinishedCurrentCurrFrame;
  ecs_raw(gEcs, HAND_ANIMATION, &entities, (void**)&handAnim, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    notStarted = handAnim[i].currentIndex == -1;
    justFinishedCurrentCurrFrame =
        handAnim[i].currentDuration > 0 && !(--handAnim[i].currentDuration);
    if (notStarted || justFinishedCurrentCurrFrame)
    {
      if (next_kframe(&handAnim[i]))
      {
        hand = ecs_get(gEcs, entities[i], HAND);
        fdir = ecs_get(gEcs, entities[i], AIM_DIRECTION);

        INVOKE_EVENT(handAnim[i].cbFrame, entities[i], handAnim[i].currentIndex);
        kf                   = handAnim[i].keyframes + handAnim[i].currentIndex;
        hand->angle          = handAnim[i].initialAngle + kf->angle * signf(fdir->value.x);
        hand->length         = handAnim[i].initialLength + kf->length;
        hand->originalPoint  = vec2_add(handAnim[i].initialOriginalPoint, kf->original);
      }
      else
      {
        INVOKE_EVENT(handAnim[i].cbCompleted, entities[i]);
        hand         = ecs_get(gEcs, entities[i], HAND);
        hand->length = handAnim[i].initialLength;
        ecs_rmv(gEcs, entities[i], HAND_ANIMATION);
      }
    }
  }
}

void hand_system(void)
{
  ecs_entity_t*     entities;
  ecs_size_t        cnt;
  Hand*             hand;
  AimDirection*     fdir;

  ecs_raw(gEcs, HAND, &entities, (void**)&hand, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (ecs_has(gEcs, entities[i], HAND_ANIMATION))
      continue;
    if (hand[i].weapon == ECS_NULL_ENT)
      continue;
    fdir          = ecs_get(gEcs, entities[i], AIM_DIRECTION);
    hand[i].angle = SDL_atan2f(fdir->value.y, fdir->value.x) * RAD_TO_DEG;

    /*
    if (hand[i].angle < -65.f && hand[i].angle > -115)
    {
      hand[i].angle = hand[i].angle > -90.f ? -65.f : -115.f;
    }
    */

#if 0
    if (ecs_has(gEcs, entities[i], PLAYER_TAG))
      printf("angle: %lf\n", hand[i].angle);
#endif
  }
}

void equipment_system()
{
  hand_system();
  hand_animation_system();
  weapon_transform_system();
}
