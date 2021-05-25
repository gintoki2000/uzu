#include "behavior_tree.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

void ai_system(void)
{
  ecs_entity_t*   entities;
  ecs_size_t      cnt;
  Brain*          brain;
  BTUpdateContext ctx = { 0 };
  ecs_raw(gEcs, BRAIN, &entities, (void**)&brain, &cnt);
  ctx.registry = gEcs;
  for (int i = 0; i < cnt; ++i)
  {
    ctx.blackboard = ecs_get(gEcs, entities[i], BLACKBOARD);
    ctx.entity     = entities[i];
    bt_node_tick(brain[i].root, &ctx);
  }
}
