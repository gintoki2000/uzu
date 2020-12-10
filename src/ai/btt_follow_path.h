#ifndef BTT_FOLLOW_PATH_H
#define BTT_FOLLOW_PATH_H
#include <behaviour_tree.h>
typedef struct
{
  __EXTEND_BT_NODE__
  int curr;
} btt_FollowPath;

btt_FollowPath* btt_follow_path_new();
btt_FollowPath* btt_follow_path_init(btt_FollowPath* self);
void            btt_follow_path_start(btt_FollowPath* self);
void            btt_follow_path_exec(btt_FollowPath* self, Ecs* ecs, ecs_entity_t entity);
#endif // BTT_FLLOW_PATH_H
