#ifndef UTILS
#define UTILS
#include <ecs/ecs.h>
ecs_entity_t get_player(Ecs* ecs);
Vec2         get_player_position(Ecs* ecs);
void         move_player_to(Ecs* ecs, Vec2 dest);
ecs_entity_t find_ladder(Ecs* ecs, const char* name);
Vec2         get_entity_position(Ecs* ecs, ecs_entity_t entity);
BOOL         equip(Ecs* ecs, ecs_entity_t entity, ecs_entity_t weapon);
ecs_entity_t find_entity(Ecs* ecs, const char* name);
#endif // UTILS
