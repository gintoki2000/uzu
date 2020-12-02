#include "ecs/ecs.h"
#include <components.h>
#include <system/movement_system.h>

static const SDL_Point DIR_TO_VEL[] = {
  [UP] = { 0, -1 }, [DOWN] = { 0, 1 }, [LEFT] = { -1, 0 }, [RIGHT] = { 1, 0 }, [NONE] = { 0, 0 },
};

void MovementSystem(Ecs* ecs)
{
  ecs_entity_t* ent;
  CmdInput*     cmd;
  Transform*    tx;
  ecs_size_t    cnt;

  ecs_data(ecs, COMMAND_INPUT, &ent, (void**)&cmd, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    tx = ecs_get(ecs, ent[i], TRANSFORM);

    tx->pos.x += DIR_TO_VEL[cmd[i].desired_dir].x * 4;
    tx->pos.y += DIR_TO_VEL[cmd[i].desired_dir].y * 4;
    cmd[i].desired_dir = NONE;
  }
}
