#include "system/game_logic.h"
#include "components.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

void life_span_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  LifeSpan*     life_span;

  ecs_raw(g_ecs, LIFE_SPAN, &entities, (void**)&life_span, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--life_span[i].remaining == 0)
    {
      ems_broadcast(MSG_LIFE_SPAN_FINISHED, &(MSG_LifeSpanFinished){ entities[i] });
      ecs_destroy(g_ecs, entities[i]);
    }
  }
}
