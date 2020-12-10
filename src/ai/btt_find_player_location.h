#ifndef BTT_FIND_PLAYER_LOCATION_H
#define BTT_FIND_PLAYER_LOCATION_H
#include "behaviour_tree.h"
typedef struct btt_FindPlayerLocation
{
  __EXTEND_BT_NODE__
} btt_FindPlayerLocation;

btt_FindPlayerLocation* btt_find_player_location_new();
btt_FindPlayerLocation* btt_find_player_location_init(btt_FindPlayerLocation* self);
void btt_find_player_location_exec(btt_FindPlayerLocation* self, Ecs* ecs, ecs_entity_t entity);
#endif // BTT_FIND_PLAYER_LOCATION_H
