#include "is_player_far_away.h"
#include <components.h>
BT_VTBL_INST_FN(bt_Condition, is_player_far_away)
BT_ALLOC_FN(IsPlayerFarAway, is_player_far_away)

void is_player_far_away_vtbl_init(bt_ConditionVtbl* vtbl)
{
  bt_condition_vtbl_init(vtbl);
  vtbl->__parent_vtbl.parent = bt_condition_vtbl_inst();
  vtbl->pred                 = (bt_pred_fn_t)is_player_far_away_pred;
}

IsPlayerFarAway* is_player_far_away_new(float radius)
{
  return is_player_far_away_init(is_player_far_away_alloc(), radius);
}

IsPlayerFarAway* is_player_far_away_init(IsPlayerFarAway* self, float radius)
{
  bt_condition_init((bt_Condition*)self, TRUE);
  self->radius = radius;
  return self;
}

static ecs_entity_t get_player(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  ecs_raw(ecs, PLAYER_TAG, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

BOOL is_player_far_away_pred(IsPlayerFarAway* self, Ecs* ecs, ecs_entity_t entity)
{
  Transform*   owner_transform;
  Transform*   player_transform;
  ecs_entity_t player;
  Vec2         dist;

  player = get_player(ecs);

  if (player == ECS_NULL_ENT)
    return FALSE;

  owner_transform  = ecs_get(ecs, entity, TRANSFORM);
  player_transform = ecs_get(ecs, player, TRANSFORM);

  dist.x = player_transform->pos.x - owner_transform->pos.x;
  dist.y = player_transform->pos.y - owner_transform->pos.y;

  return vec2_mag(dist) > self->radius;
}
