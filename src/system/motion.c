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

  const MoveSpeed*  move_speed;
  DesiredDirection* desired_direction;
  Motion*           motion;

  ecs_raw(g_ecs, DESIRED_DIRECTION, &entities, (void**)&desired_direction, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(g_ecs, entities[i], UNABLE_TO_MOVE) &&
        (motion = ecs_get(g_ecs, entities[i], MOTION)) &&
        (move_speed = ecs_get(g_ecs, entities[i], MOVE_SPEED)))
    {
      motion->vel          = vec2_mul(desired_direction[i], (float)move_speed->value);
      desired_direction[i] = VEC2_ZERO;
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

    transform->lastPosition = transform->position;
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
