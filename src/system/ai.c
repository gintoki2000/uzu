#include "behavior_tree.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/game_logic.h"

extern ecs_Registry* gRegistry;

void ai_system(void)
{
  ecs_entity_t*   entities;
  ecs_size_t      cnt;
  Brain*          brain;
  bt_UpdateContext ctx = { 0 };
  ecs_raw(gRegistry, BRAIN, &entities, (void**)&brain, &cnt);
  ctx.registry = gRegistry;
  for (int i = 0; i < cnt; ++i)
  {
  }
}
