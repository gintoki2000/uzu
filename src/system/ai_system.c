#include "ai_system.h"
#include "behaviour_tree.h"
#include "components.h"

void sys_ai_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  AIAgent*      ai_agent;

  ecs_data(ecs, AI_AGENT, &entities, (void**)&ai_agent, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    bt_node_vc_exec(ai_agent[i].root, ecs, entities[i]);
  }
}
