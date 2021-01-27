#include "animator_ctl_sys.h"

#include <components.h>

extern Ecs* g_ecs;

void animator_controller_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Animator*     animators;
  Motion*       motion;
  Visual*       visual;

  float          vx, vy;
  AnimationState next_state;

  ecs_raw(g_ecs, ANIMATOR, &entities, (void**)&animators, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (visual = ecs_get(g_ecs, entities[i], VISUAL)))
    {

      if (ecs_has(g_ecs, entities[i], INVULNERABLE))
      {
        animators[i].current_anim = ANIM_STATE_HIT;
        animators[i].elapsed      = 0;
        return;
      }
      vx = motion->vel.x;
      vy = motion->vel.y;

      next_state = (absf(vx) > 0.1f || absf(vy) > 0.1f) ? ANIM_STATE_RUN : ANIM_STATE_IDLE;
      if (next_state != animators[i].current_anim)
      {
        animators[i].current_anim = next_state;
        animators[i].elapsed      = 0;
      }

      if (absf(vx) > 0.1f)
      {
        visual->flip = vx > 0.f ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
      }
    }
  }
}
