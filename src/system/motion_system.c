#include "motion_system.h"
#include <components.h>
#include <toolbox/toolbox.h>
#define DT 0.0166f

void MotionSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion;

  ecs_data(ecs, MOTION, &entities, (void**)&motion, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);

    transform->prev_pos = transform->pos;
    transform->pos.x += motion[i].vel.x * DT;
    transform->pos.y += motion[i].vel.y * DT;

    transform->was_changed = (absf(transform->pos.x - transform->prev_pos.x) > 0.1f ||
                              absf(transform->pos.y - transform->prev_pos.y) > 0.1f);
  }
}
