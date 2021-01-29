#include "ai_system.h"
#include <behaviour_tree.h>
#include <components.h>
#include <ecs/ecs.h>

extern Ecs* g_ecs;

void ai_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Brain*      ai_agent;

  ecs_raw(g_ecs, BRAIN, &entities, (void**)&ai_agent, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(g_ecs, entities[i], INPUT_BLOCKER))
      bt_node_vc_exec(ai_agent[i].root, g_ecs, entities[i]);
  }
}
