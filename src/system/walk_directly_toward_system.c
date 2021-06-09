#include "components.h"
#include "ecs/ecs.h"
#define RADIUS_SQUARE 9
void walk_directly_toward_system(void)
{
  extern ecs_Registry*         gRegistry;
  ecs_entity_t*       entities;
  ecs_size_t          cnt;
  WalkDirectlyToward* walkDirectlyToward;
  DesiredDirection*   desiredDirection;
  Transform*          transform;
  Vec2                relativePosition;

  ecs_raw(gRegistry, WALK_DIRECTLY_TOWARD, &entities, (void**)&walkDirectlyToward, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((desiredDirection = ecs_get(gRegistry, entities[i], DESIRED_DIRECTION)) &&
        (transform = ecs_get(gRegistry, entities[i], TRANSFORM)))
    {
      relativePosition = vec2_sub(walkDirectlyToward[i].destination, transform->position);
      if (vec2_mag2(relativePosition) <= RADIUS_SQUARE)
      {
        transform->lastPosition = transform->position;
        transform->position     = walkDirectlyToward[i].destination;
        *desiredDirection       = VEC2_ZERO;
        INVOKE_EVENT(walkDirectlyToward[i].cbCompleted, TRUE);
        ecs_rmv(gRegistry, entities[i], WALK_DIRECTLY_TOWARD);
        continue;
      }
      *desiredDirection = vec2_unit(relativePosition);
    }
  }
}
