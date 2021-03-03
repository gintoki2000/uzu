#include "components.h"
#include "system/game_logic.h"

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

void character_controller_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  Animator*        animator;
  Motion*          motion;
  Visual*          visual;
  FacingDirection* facing_direction;

  float          vx, vy;
  AnimationState next_state;

  ecs_raw(g_ecs, CHARACTER_ANIMATOR_TAG, &entities, NULL, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (visual = ecs_get(g_ecs, entities[i], VISUAL)) &&
        (animator = ecs_get(g_ecs, entities[i], ANIMATOR)) &&
        (facing_direction = ecs_get(g_ecs, entities[i], FACING_DIRECTION)))
    {

      if (ecs_has(g_ecs, entities[i], INVULNERABLE))
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

      next_state = (absf(vx) > 0.1f || absf(vy) > 0.1f) ? ANIM_STATE_RUN : ANIM_STATE_IDLE;

      play_animation(animator, next_state);
      if (ecs_has(g_ecs, entities[i], PLAYER_TAG))
        SDL_Log("%f, %f", facing_direction->value.x, facing_direction->value.y);
      visual->flip = signf(facing_direction->value.x) < 0 ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    }
  }
}
