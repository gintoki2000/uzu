#include "motion_system.h"
#include <components.h>
#define DT 0.033f

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
    transform->pos.x += motion[i].vel.x * DT;
    transform->pos.y += motion[i].vel.y * DT;
  }
}
