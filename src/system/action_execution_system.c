#include "action_execution_system.h"
#include "mediator.h"
#include <components.h>

static const Vec2 DIR_TO_VEL[] = {
  [UP] = VEC2_UP, [DOWN] = VEC2_DOWN, [LEFT] = VEC2_LEFT, [RIGHT] = VEC2_RIGHT, [NONE] = VEC2_ZERO,
};

void ActionExecutionSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  CharacterAction* actions;
  Motion*          motion;

  ecs_data(ecs, CHARACTER_ACTION, &entities, (void**)&actions, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion = ecs_get(ecs, entities[i], MOTION);

    if (motion)
    {

      motion->vel.x = motion->max_speed * DIR_TO_VEL[actions[i].desired_dir].x;
      motion->vel.y = motion->max_speed * DIR_TO_VEL[actions[i].desired_dir].y;

      actions[i].desired_dir = NONE;
    }
  }
}
