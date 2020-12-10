#include "btt_follow_path.h"

BT_DEF_NODE_VTBL(BTT_FOLLOW_PATH_VTBL, bt_node_fini, bt_node_start_impl, btt_fllow_path_exec);

btt_FollowPath* btt_fllow_path_new()
{
  btt_FollowPath* node = malloc(sizeof(btt_FollowPath));
  BT_NODE(node)->vtbl  = &BTT_FOLLOW_PATH_VTBL;
  return btt_fllow_path_init(node);
}

btt_FollowPath* btt_fllow_path_init(btt_FollowPath *self)
{
  bt_node_init((bt_Node*) self);
  return self; 
}

void btt_fllow_path_exec(btt_FollowPath *self, Ecs *ecs, ecs_entity_t entity)
{
   
  bt_finish_exec(self, BT_STATUS_SUCCESS);
}
