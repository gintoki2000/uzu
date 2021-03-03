#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void paralyzing_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Paralyzed* paralyzed;
  ecs_raw(g_ecs, PARALYZED, &entities, (void**)&paralyzed, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (paralyzed[i].remaining > 0 && (--paralyzed[i].remaining == 0))
    {
      set_entity_velocity(g_ecs, entities[i], VEC2_ZERO);
      ecs_rmv(g_ecs, entities[i], PARALYZED);
    }
  }
}
