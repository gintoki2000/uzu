#include "motion_system.h"
#include <components.h>
#include <toolbox/toolbox.h>
#define DT 0.0166f

void MotionSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion_ls;

  ecs_data(ecs, MOTION, &entities, (void**)&motion_ls, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion_ls[i].vel.x += motion_ls[i].acc.x * DT;
    motion_ls[i].vel.y += motion_ls[i].acc.y * DT;
    motion_ls[i].vel = truncatev(motion_ls[i].vel, 100);
    motion_ls[i].acc = VEC2_ZERO;
  }

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);

    transform->prev_pos = transform->pos;
    transform->pos.x += motion_ls[i].vel.x * DT;
    transform->pos.y += motion_ls[i].vel.y * DT;

    transform->was_changed = (absf(transform->pos.x - transform->prev_pos.x) > 0.1f ||
                              absf(transform->pos.y - transform->prev_pos.y) > 0.1f);
  }
}
