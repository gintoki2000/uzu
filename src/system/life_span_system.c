#include "life_span_system.h"
#include "mediator.h"
#include <components.h>

void LifeSpanSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  LifeSpan*     life_span;

  ecs_data(ecs, LIFE_SPAN, &entities, (void**)&life_span, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--life_span[i].remaining == 0)
    {
      INFO("e"ECS_ENT_FMT_PATTERN" end life span\n", ECS_ENT_FMT_VARS(entities[i]));
      mediator_emit(SIG_LIFE_SPAN_END_UP, &(SysEvt_LifeSpanEndUp){ entities[i] });
      ecs_destroy(ecs, entities[i]);
    }
  }
}
