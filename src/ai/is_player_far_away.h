#ifndef BTC_IS_PLAYER_FAR_AWAY_H
#define BTC_IS_PLAYER_FAR_AWAY_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(BTCondition)
  float radius;
} IsPlayerFarAway;

const BTNodeVtbl* is_player_far_away_vtbl_inst();
void               is_player_far_away_vtbl_init(BTConditionVtbl* vtbl);

IsPlayerFarAway* is_player_far_away_new(float radius);
IsPlayerFarAway* is_player_far_away_init(IsPlayerFarAway* self, float radius);
BOOL is_player_far_away_pred(IsPlayerFarAway* self, Ecs* ecs, ecs_entity_t entity);

#endif // BTC_IS_PLAYER_FAR_AWAY_H
