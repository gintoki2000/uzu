#include "utils.h"
#include <components.h>
ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_raw(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

Vec2 get_player_position(Ecs* ecs)
{
  return get_entity_position(ecs, get_player(ecs));
}

void move_player_to(Ecs* ecs, Vec2 dest)
{
  ecs_entity_t player = get_player(ecs);
  ASSERT(player != ECS_NULL_ENT);
  Transform* transform = ecs_get(ecs, player, TRANSFORM);
  transform->pos       = dest;
}

Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  return ((Transform*)ecs_get(ecs, entity, TRANSFORM))->pos;
}

