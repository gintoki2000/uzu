#ifndef BTT_IS_PLAYER_NEAR_BY_H
#define BTT_IS_PLAYER_NEAR_BY_H
#include <behaviour_tree.h>
typedef struct btt_IsPlayerNearBy
{
  __EXTEND_BT_NODE__
} btt_IsPlayerNearBy;

btt_IsPlayerNearBy* btt_is_player_near_by_new();
btt_IsPlayerNearBy* btt_is_player_near_by_init(btt_IsPlayerNearBy* self);

#endif // BTT_IS_PLAYER_NEAR_BY_H
