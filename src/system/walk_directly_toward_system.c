#include "components.h"
#include "ecs/ecs.h"
#define RADIUS_SQUARE 9
void walk_directly_toward_system(void)
{
  extern Ecs*         g_ecs;
  ecs_entity_t*       entities;
  ecs_size_t          cnt;
  WalkDirectlyToward* walk_directly_toward;
  DesiredDirection*   desired_direction;
  Transform*          transform;
  Vec2                relative_position;

  ecs_raw(g_ecs, WALK_DIRECTLY_TOWARD, &entities, (void**)&walk_directly_toward, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((desired_direction = ecs_get(g_ecs, entities[i], DESIRED_DIRECTION)) &&
        (transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      relative_position = vec2_sub(transform->position, walk_directly_toward[i].destination);
      if (vec2_mag2(relative_position) <= RADIUS_SQUARE)
      {
        transform->prev_position = transform->position;
        transform->position      = walk_directly_toward[i].destination;
        *desired_direction       = VEC2_ZERO;
        INVOKE_EVENT(walk_directly_toward[i].cbCompleted, TRUE);
        ecs_rmv(g_ecs, entities[i], WALK_DIRECTLY_TOWARD);
        continue;
      }
      *desired_direction = vec2_unit(relative_position);
    }
  }
}
