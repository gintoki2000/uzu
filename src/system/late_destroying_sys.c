#include "late_destroying_sys.h"

#include <components.h>
#include <toolbox/toolbox.h>

void sys_late_destroying_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Equipment* equipment;

  ecs_data(ecs, TAG_TO_BE_DESTROYED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((equipment = ecs_get(ecs, entities[i], EQUIPMENT)))
    {
      ecs_destroy(ecs, equipment->weapon);
    }
    ecs_destroy(ecs, entities[i]);
  }
}
