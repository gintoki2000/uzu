#include "utils.h"
#include <components.h>
ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_data(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

Vec2 get_player_position(Ecs* ecs)
{
  Transform* transform = ecs_get(ecs, get_player(ecs), TRANSFORM);
  return transform->pos;
}
