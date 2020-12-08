#ifndef BTT_FIND_RANDOM_LOCATION_H
#define BTT_FIND_RANDOM_LOCATION_H
#include <behaviour_tree.h>
typedef struct btt_FindRandomLocation
{
  __EXTEND_BT_NODE__
} btt_FindRandomLocation;

btt_FindRandomLocation* btt_find_random_location_new();
btt_FindRandomLocation* btt_find_random_location_init(btt_FindRandomLocation* self);
void btt_find_random_location_exec(btt_FindRandomLocation* self, Ecs* ecs, ecs_entity_t entity);
#endif // BTT_FIND_RANDOM_LOCATION_H
