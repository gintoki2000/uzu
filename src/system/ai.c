#include "behavior_tree.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

void ai_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Brain*        ai_agent;

  ecs_raw(g_ecs, BRAIN, &entities, (void**)&ai_agent, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
  }
}
