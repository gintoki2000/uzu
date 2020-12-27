#include "ai_system.h"
#include <behaviour_tree.h>
#include <components.h>
#include <ecs/ecs.h>

extern Ecs* g_ecs;

void ai_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  AIAgent*      ai_agent;

  ecs_raw(g_ecs, AI_AGENT, &entities, (void**)&ai_agent, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    bt_node_vc_exec(ai_agent[i].root, g_ecs, entities[i]);
  }
}
