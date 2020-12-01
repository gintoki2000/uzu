#include "life_span_system.h"
#include <components.h>

void LifeSpanSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  LifeSpan*     life_span;

  ecs_data(ecs, LIFE_SPAN, &entities, (void**)&life_span, &cnt);

  if (cnt > 0)
    for (int i = cnt - 1; i; --i)
    {
      if (life_span[i].remaining > 0)
      {
        --life_span[i].remaining;
        if (life_span[i].remaining == 0)
        {
          INFO("e{%3u | %3u} end life span\n", ECS_ENT_IDX(entities[i]), ECS_ENT_VER(entities[i]));
          ecs_destroy(ecs, entities[i]);
        }
      }
    }
}
