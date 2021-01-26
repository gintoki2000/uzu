#include "block_input_system.h"
#include <components.h>

extern Ecs* g_ecs;

void input_blocking_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  InputBlocker* input_blockers;
  ecs_raw(g_ecs, INPUT_BLOCKER, &entities, (void**)&input_blockers, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--input_blockers[i].remaining == 0)
      ecs_rmv(g_ecs, entities[i], INPUT_BLOCKER);
  }
}
