#include "btt_find_random_location.h"
#include <components.h>
#include <constances.h>

static const bt_NodeVtbl BTT_FIND_RANDOM_LOCATION_VTBL = {
  .fini  = (destroy_fn_t)bt_node_fini,
  .start = (bt_start_fn_t)bt_node_start_impl,
  .exec  = (bt_exec_fn_t)btt_find_random_location_exec,
};

btt_FindRandomLocation* btt_find_random_location_new()
{
  btt_FindRandomLocation* task = malloc(sizeof(btt_FindRandomLocation));
  BT_NODE(task)->vtbl          = &BTT_FIND_RANDOM_LOCATION_VTBL;
  return btt_find_random_location_init(task);
}

btt_FindRandomLocation* btt_find_random_location_init(btt_FindRandomLocation* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

void btt_find_random_location_exec(btt_FindRandomLocation* self, Ecs* ecs, ecs_entity_t entity)
{
  //INFO("executing...\n");
  btv_MoveDestination* dest;
  dest    = ecs_add(ecs, entity, BTV_MOVE_DESTINATION);
  dest->x = rand() % WIN_WIDTH;
  dest->y = rand() % WIN_HEIGHT;
  bt_node_set_stt(self, BT_STATUS_SUCCESS);
}
