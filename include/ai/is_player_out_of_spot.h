#ifndef IS_PLAYER_OUT_OF_SPOT_H
#define IS_PLAYER_OUT_OF_SPOT_H
#include "behaviour_tree.h"
typedef struct
{
  BT_EXTEND_NODE(BTCondition)
} BTCondition_IsPlayerOutOfSpot;

BTCondition_IsPlayerOutOfSpot* bt_condition_is_player_out_of_spot_new();
#endif // IS_PLAYER_OUT_OF_SPOT_H
