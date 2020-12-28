#include "find_player_target.h"
#include <behaviour_tree.h>
#include <components.h>
#include <utils.h>
struct FindPlayerTarget
{
  BT_EXTEND_NODE(BTNode)
};

const BTNodeVtbl*       find_player_target_vtbl_inst();
static void              find_player_target_vtbl_init(BTNodeVtbl* vbtl);
static FindPlayerTarget* find_player_target_init(FindPlayerTarget* self);
static BTStatus         find_player_target_exec(FindPlayerTarget* self, Ecs*, ecs_entity_t);

BT_VTBL_INST_FN(BTNode, find_player_target)
BT_ALLOC_FN(FindPlayerTarget, find_player_target)

FindPlayerTarget* find_player_target_new()
{
  return find_player_target_init(find_player_target_alloc());
}

static void find_player_target_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)find_player_target_exec;
}

static FindPlayerTarget* find_player_target_init(FindPlayerTarget* self)
{
  bt_node_init((BTNode*)self);
  return self;
}

static BTStatus find_player_target_exec(FindPlayerTarget* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;
  ecs_entity_t player;
  Transform*   player_transform;
  Transform*   transform;
  if ((player = get_player(ecs)) == ECS_NULL_ENT)
    return BT_STATUS_FAILURE;

  if ((player_transform = ecs_get(ecs, player, TRANSFORM)) == NULL ||
      (transform = ecs_get(ecs, entity, TRANSFORM)) == NULL)
    return BT_STATUS_FAILURE;

  if (vec2_mag(vec2_sub(player_transform->pos, transform->pos)) < 16.f)
    return BT_STATUS_FAILURE;

  *((FollowingTarget*)ecs_add(ecs, entity, FOLLOWING_TARGET)) = player;
  return BT_STATUS_SUCCESS;
}
