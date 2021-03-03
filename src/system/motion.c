#include "system/game_logic.h"

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

  const Controller* controllers;
  const MoveSpeed*  move_speed;
  Motion*           motion;

  ecs_raw(g_ecs, CONTROLLER, &entities, (void**)&controllers, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(g_ecs, entities[i], PARALYZED) && (motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (move_speed = ecs_get(g_ecs, entities[i], MOVE_SPEED)))
    {
      motion->vel = vec2_mul(controllers[i].desired_direction, (float)move_speed->value);
    }
  }
}

void motion_system()
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion;

  apply_controller_input();

  ecs_raw(g_ecs, MOTION, &entities, (void**)&motion, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion[i].vel.x += motion[i].acc.x;
    motion[i].vel.y += motion[i].acc.y;
    motion[i].vel = vec2_trunc(motion[i].vel, SPEED_LIMIT);
    motion[i].acc = VEC2_ZERO;
  }

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);

    transform->prev_position = transform->position;
    transform->position.x += motion[i].vel.x * DT;
    transform->position.y += motion[i].vel.y * DT;

    if (absf(motion[i].vz) < 0.1f)
      motion[i].vz = 0.f;
    transform->z += motion[i].vz * DT;
    if (transform->z < 0.f)
    {
      motion[i].vz *= -motion[i].bounching;
      transform->z = 0.f;
    }
  }

  for (int i = 0; i < cnt; ++i)
  {
    motion[i].vel = vec2_mul(motion[i].vel, 1.f - motion[i].friction);
    motion[i].vz += GRAVITY;
  }
}
