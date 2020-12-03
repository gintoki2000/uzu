#include "late_destroying_system.h"
#include <toolbox/toolbox.h>
#include <components.h>

void LateDestroyingSystem(Ecs *ecs)
{
  ecs_entity_t* entities;
  ecs_size_t cnt;
  
  ecs_data(ecs, TAG_TO_BE_DESTROYED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    ecs_destroy(ecs, entities[i]);
  } 
}
