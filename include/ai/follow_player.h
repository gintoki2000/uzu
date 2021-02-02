#define FOLLOW_PLAYER_H
#ifdef FOLLOW_PLAYER_H
#include "behaviour_tree.h"

typedef struct
{
  BT_EXTEND_NODE(BTNode)
  BOOL is_running;
  float radius;
} BTTask_FollowPlayer;

BTTask_FollowPlayer* bt_task_follow_player_new(float radius);

#endif
