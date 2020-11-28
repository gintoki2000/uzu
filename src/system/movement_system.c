#include "ecs/world.h"
#include <components.h>
#include <system/movement_system.h>

static const SDL_Point dir_to_vel[] = {
  [UP] = {0, -1},
  [DOWN] = {0, 1},
  [LEFT] = {-1, 0},
  [RIGHT] = {1, 0},
  [NONE] = {0, 0}
};

void MovementSystem(Ecs* ecs)
{
  ecs_entity_t* ent;
  CommandInput* cmd;
  Transform*    tx;
  ecs_size_t    cnt;

  ecs_data(ecs, COMMAND_INPUT, &ent, (void**)&cmd, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    tx = ecs_get(ecs, ent[i], TRANSFORM);

    tx->x += dir_to_vel[cmd[i].desired_dir].x * 4;
    tx->y += dir_to_vel[cmd[i].desired_dir].y * 4;
    cmd[i].desired_dir = NONE;
  }
}
