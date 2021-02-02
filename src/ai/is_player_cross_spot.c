#include "is_player_cross_spot.h"
#include <behaviour_tree.h>
#include <components.h>
#include <utils.h>
struct IsPlayerCrossSpot
{
  BT_EXTEND_NODE(BTCondition)
};

const BTNodeVtbl* is_player_cross_spot_vtbl_inst();
void               is_player_cross_spot_vtbl_init(BTConditionVtbl* vtbl);
BTCondition_IsPlayerCrossSpot* is_player_cross_spot_init(BTCondition_IsPlayerCrossSpot* self);
BOOL is_player_cross_spot_pred(BTCondition_IsPlayerCrossSpot* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(BTCondition, is_player_cross_spot)
BT_ALLOC_FN(BTCondition_IsPlayerCrossSpot, is_player_cross_spot)

void is_player_cross_spot_vtbl_init(BTConditionVtbl* vtbl)
{
  bt_condition_vtbl_init(vtbl);
  vtbl->pred = (bt_pred_fn_t)is_player_cross_spot_pred;
}

BTCondition_IsPlayerCrossSpot* is_player_cross_spot_new()
{
  return is_player_cross_spot_init(is_player_cross_spot_alloc());
}

BTCondition_IsPlayerCrossSpot* is_player_cross_spot_init(BTCondition_IsPlayerCrossSpot* self)
{
  bt_condition_init((BTCondition*)self, TRUE);
  return self;
}

BOOL is_player_cross_spot_pred(BTCondition_IsPlayerCrossSpot* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  ecs_entity_t player;
  Transform*   player_transform;
  Spot*        spot;

  if ((player = get_player(ecs)) == ECS_NULL_ENT)
    return FALSE;

  player_transform = ecs_get(ecs, player, TRANSFORM);
  spot             = ecs_get(ecs, entity, SPOT);

  ASSERT(spot != NULL);
  ASSERT(player_transform != NULL);

  return vec2_mag(vec2_sub(player_transform->position, spot->position)) < spot->radius;
}
