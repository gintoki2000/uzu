#include "late_destroying_sys.h"

#include <components.h>
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

void late_destroying_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Equipment* equipment;

  ecs_raw(g_ecs, TAG_TO_BE_DESTROYED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((equipment = ecs_get(g_ecs, entities[i], EQUIPMENT)))
    {
      ecs_destroy(g_ecs, equipment->weapon);
    }
    ecs_destroy(g_ecs, entities[i]);
  }
}
