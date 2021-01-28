#include "animator_ctl_sys.h"

#include <components.h>

extern Ecs* g_ecs;

void animator_controller_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Animator*     animator;
  Motion*       motion;
  Visual*       visual;

  float          vx, vy;
  AnimationState next_state;

  ecs_raw(g_ecs, CHARACTER_ANIMATOR_TAG, &entities, NULL, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (visual = ecs_get(g_ecs, entities[i], VISUAL)) &&
        (animator = ecs_get(g_ecs, entities[i], ANIMATOR)))
    {

      if (ecs_has(g_ecs, entities[i], INVULNERABLE))
      {
        if (animator->current_anim != ANIM_STATE_HIT)
        {
          animator->current_anim = ANIM_STATE_HIT;
          animator->elapsed      = 0;
        }
        return;
      }
      vx = motion->vel.x;
      vy = motion->vel.y;

      next_state = (absf(vx) > 0.1f || absf(vy) > 0.1f) ? ANIM_STATE_RUN : ANIM_STATE_IDLE;
      if (next_state != animator->current_anim)
      {
        animator->current_anim = next_state;
        animator->elapsed      = 0;
      }

      if (absf(vx) > 0.1f)
      {
        visual->flip = vx > 0.f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
      }
    }
  }
}
