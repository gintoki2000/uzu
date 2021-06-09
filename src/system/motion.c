#include "system/game_logic.h"

#include "components.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

#define DT 0.0166f
#define SPEED_LIMIT 400.f
#define GRAVITY -6.f

extern ecs_Registry* gRegistry;

static void move(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  const MoveSpeed*  moveSpeed;
  DesiredDirection* desiredDirection;
  Motion*           motion;

  ecs_raw(gRegistry, DESIRED_DIRECTION, &entities, (void**)&desiredDirection, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(gRegistry, entities[i], UNABLE_TO_MOVE) &&
        (motion = ecs_get(gRegistry, entities[i], MOTION)) &&
        (moveSpeed = ecs_get(gRegistry, entities[i], MOVE_SPEED)))
    {
      motion->vel         = vec2_mul(desiredDirection[i], (float)moveSpeed->value);
      desiredDirection[i] = VEC2_ZERO;
    }
  }
}

void motion_system()
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  Motion*    motion;

  move();

  ecs_raw(gRegistry, MOTION, &entities, (void**)&motion, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    motion[i].vel.x += motion[i].acc.x;
    motion[i].vel.y += motion[i].acc.y;
    motion[i].vel = vec2_trunc(motion[i].vel, SPEED_LIMIT);
    motion[i].acc = VEC2_ZERO;
  }

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(gRegistry, entities[i], TRANSFORM);

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
