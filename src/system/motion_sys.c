#include "motion_sys.h"

#include <components.h>
#include <toolbox/toolbox.h>
#define DT 0.0166f

static void set_vel_by_controller_input(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Controller* controllers;
  Motion*     motion;

  ecs_data(ecs, CONTROLLER, &entities, (void**)&controllers, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!controllers[i].lock_movement)
    {
      if ((motion = ecs_get(ecs, entities[i], MOTION)))
      {
        motion->vel = controllers[i].desired_vel;
      }
    }
  }
}

void sys_motion_update(Ecs* ecs)
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion_ls;

  set_vel_by_controller_input(ecs);

  ecs_data(ecs, MOTION, &entities, (void**)&motion_ls, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion_ls[i].vel.x += motion_ls[i].acc.x;
    motion_ls[i].vel.y += motion_ls[i].acc.y;
    motion_ls[i].vel = vec2_trunc(motion_ls[i].vel, motion_ls[i].max_speed);
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
