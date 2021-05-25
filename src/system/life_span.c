#include "components.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

void life_span_system()
{
  extern Ecs*   gEcs;
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  LifeSpan*     lifeSpan;

  ecs_raw(gEcs, LIFE_SPAN, &entities, (void**)&lifeSpan, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--lifeSpan[i].remaining == 0)
    {
      ems_broadcast(MSG_LIFE_SPAN_FINISHED, &(LifeSpanFinishedMsg){ entities[i] });
      ecs_destroy(gEcs, entities[i]);
    }
  }
}
