#include "ai_system.h"
#include "behaviour_tree.h"
#include "components.h"

void AiSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  AIAgent*      ai_agent;

  ecs_data(ecs, AI_AGENT, &entities, (void**)&ai_agent, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    //printf(">> begin tick bt\n");
    if (ai_agent[i].root->status == BT_STATUS_NOT_EXECUTED)
    {
      bt_node_start(ai_agent[i].root);
    }

    bt_node_exec(ai_agent[i].root, ecs, entities[i]);
  }
}
