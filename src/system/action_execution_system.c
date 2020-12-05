#include "action_execution_system.h"
#include "mediator.h"
#include <components.h>

static const Vec2 DIR_TO_VEL[] = {
  [UP] = VEC2_UP, [DOWN] = VEC2_DOWN, [LEFT] = VEC2_LEFT, [RIGHT] = VEC2_RIGHT, [NONE] = VEC2_ZERO,
};

static const int DIR_TO_ANIMATION[] = {
  [UP] = ANIM_STATE_RUN,    [DOWN] = ANIM_STATE_RUN,  [LEFT] = ANIM_STATE_RUN,
  [RIGHT] = ANIM_STATE_RUN, [NONE] = ANIM_STATE_IDLE,
};

#define UNCHANGE -1
static const int DIR_TO_FLIP[] = {
  [UP] = UNCHANGE,         [DOWN] = UNCHANGE, [LEFT] = SDL_FLIP_HORIZONTAL,
  [RIGHT] = SDL_FLIP_NONE, [NONE] = UNCHANGE,
};
void ActionExecutionSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  CharacterAction* actions;
  Motion*          motion;
  CharacterStats*  stats;
  Animator*        animator;
  Visual*          visual;

  int prev_anim;
  int next_anim;

  ecs_data(ecs, CHARACTER_ACTION, &entities, (void**)&actions, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion   = ecs_get(ecs, entities[i], MOTION);
    stats    = ecs_get(ecs, entities[i], CHARACTER_STATS);
    animator = ecs_get(ecs, entities[i], ANIMATOR);
    visual   = ecs_get(ecs, entities[i], VISUAL);

    motion->vel.x = stats->move_speed * DIR_TO_VEL[actions[i].desired_dir].x;
    motion->vel.y = stats->move_speed * DIR_TO_VEL[actions[i].desired_dir].y;

    prev_anim = animator->current_anim;
    next_anim = DIR_TO_ANIMATION[actions[i].desired_dir];

    if (next_anim != prev_anim)
    {
      animator->current_anim = next_anim;
      animator->elapsed      = 0;
    }

    if (DIR_TO_FLIP[actions[i].desired_dir] != UNCHANGE)
    {
      visual->flip = DIR_TO_FLIP[actions[i].desired_dir];
    }

    actions[i].desired_dir = NONE;
  }
}
