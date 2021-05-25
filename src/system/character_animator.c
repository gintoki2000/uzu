#include "components.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

static void play_animation(Animator* animator, u32 newAnimState)
{
  if (animator->currentAnimation != newAnimState)
  {
    animator->currentAnimation = newAnimState;
    animator->elapsed      = 0;
  }
}

static void force_play_animation(Animator* animator, u32 newAnimState)
{
  animator->currentAnimation = newAnimState;
  animator->elapsed      = 0;
}

void character_controller_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  Animator*        animator;
  Motion*          motion;
  Visual*          visual;
  AimDirection*    aimDirection;

  float          vx, vy;
  AnimationState nextState;

  ecs_raw(gEcs, CHARACTER_ANIMATOR_TAG, &entities, NULL, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((motion = ecs_get(gEcs, entities[i], MOTION)) &&
        (visual = ecs_get(gEcs, entities[i], VISUAL)) &&
        (animator = ecs_get(gEcs, entities[i], ANIMATOR)) &&
        (aimDirection = ecs_get(gEcs, entities[i], AIM_DIRECTION)))
    {

      if (ecs_has(gEcs, entities[i], INVULNERABLE) || ecs_has(gEcs, entities[i], STAGGER))
      {
        play_animation(animator, ANIM_STATE_HIT);
        return;
      }

      if (motion->vz > .1f)
      {
        force_play_animation(animator, ANIM_STATE_JUMP);
        return;
      }
      vx = motion->vel.x;
      vy = motion->vel.y;

      nextState = (absf(vx) > 0.1f || absf(vy) > 0.1f) ? ANIM_STATE_RUN : ANIM_STATE_IDLE;

      play_animation(animator, nextState);
      visual->flip = signf(aimDirection->value.x) < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    }
  }
}
