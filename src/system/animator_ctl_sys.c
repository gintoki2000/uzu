#include "system/animator_ctl_sys.h"

#include "components.h"

extern Ecs* g_ecs;

static void play_animation(Animator* animator, u16 new_anim_state)
{
  if (animator->current_anim != new_anim_state)
  {
    animator->current_anim = new_anim_state;
    animator->elapsed      = 0;
  }
}

static void force_play_animation(Animator* animator, u16 new_anim_state)
{
  animator->current_anim = new_anim_state;
  animator->elapsed      = 0;
}

void animator_controller_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Animator*     animator;
  Motion*       motion;
  Visual*       visual;
  Transform*    transform;

  float          vx, vy;
  AnimationState next_state;

  ecs_raw(g_ecs, CHARACTER_ANIMATOR_TAG, &entities, NULL, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (visual = ecs_get(g_ecs, entities[i], VISUAL)) &&
        (animator = ecs_get(g_ecs, entities[i], ANIMATOR)) &&
        (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {

      if (ecs_has(g_ecs, entities[i], INVULNERABLE))
      {
        play_animation(animator, ANIM_STATE_HIT);
        return;
      }

      if (motion->vz > 1.f)
      {
        force_play_animation(animator, ANIM_STATE_JUMP);
        return;
      }
      vx = motion->vel.x;
      vy = motion->vel.y;

      next_state = (absf(vx) > 0.1f || absf(vy) > 0.1f) ? ANIM_STATE_RUN : ANIM_STATE_IDLE;

      play_animation(animator, next_state);
      visual->flip = transform->hdir < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    }
  }
}
