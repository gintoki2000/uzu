#include "system/block_input_system.h"

#include "../../include/entity_utils.h"
#include "components.h"

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
    {
      set_entity_velocity(g_ecs, entities[i], VEC2_ZERO);
      ecs_rmv(g_ecs, entities[i], INPUT_BLOCKER);
    }
  }
}
