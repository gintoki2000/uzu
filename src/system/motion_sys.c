#include "system/motion_sys.h"

#include "components.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

#define DT 0.0166f
#define SPEED_LIMIT 200.f
#define GRAVITY -6.f

extern Ecs* g_ecs;

static void apply_controller_input(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Controller* controllers;
  Motion*     motion;

  ecs_raw(g_ecs, CONTROLLER, &entities, (void**)&controllers, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(g_ecs, entities[i], INPUT_BLOCKER))
    {
      if ((motion = ecs_get(g_ecs, entities[i], MOTION)))
      {
        motion->vel.x += controllers[i].force.x;
        motion->vel.y += controllers[i].force.y;
        controllers[i].force = VEC2_ZERO;
      }
    }
  }
}

void motion_system_update()
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion_raw_array;

  apply_controller_input();

  ecs_raw(g_ecs, MOTION, &entities, (void**)&motion_raw_array, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion_raw_array[i].vel.x += motion_raw_array[i].acc.x;
    motion_raw_array[i].vel.y += motion_raw_array[i].acc.y;
    motion_raw_array[i].vel = vec2_trunc(motion_raw_array[i].vel, motion_raw_array[i].max_speed);
    motion_raw_array[i].acc = VEC2_ZERO;
  }

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);

    transform->prev_position = transform->position;
    transform->position.x += motion_raw_array[i].vel.x * DT;
    transform->position.y += motion_raw_array[i].vel.y * DT;
    transform->z += motion_raw_array[i].vz * DT;
    if (transform->z < 0.f)
    {
      motion_raw_array[i].vz = 0.f;
      transform->z           = 0.f;
    }

    if (!ecs_has(g_ecs, entities[i], INPUT_BLOCKER) && absf(motion_raw_array[i].vel.x) > 0.1f)
    {
      transform->hdir = motion_raw_array[i].vel.x > 0.f ? 1 : -1;
    }
  }

  for (int i = 0; i < cnt; ++i)
  {
    motion_raw_array[i].vel = vec2_mul(motion_raw_array[i].vel, 1.f - motion_raw_array[i].friction);
    motion_raw_array[i].vz += GRAVITY;
  }
}
