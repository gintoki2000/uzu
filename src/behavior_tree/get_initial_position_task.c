#include "behavior_tree/base.h"
#include "components.h"

BT_INST_DECL(GetInitialPositionTask, BTNode, { u32 bbResultKey; })
typedef BTNodeVtbl GetInitialPositionTaskVtbl;

static GetInitialPositionTask* init(GetInitialPositionTask*self, u32 resultKey)
{
  bt_node_init(BT_NODE(self));
  self->bbResultKey = resultKey;
  return self;
}

static BTStatus on_tick(GetInitialPositionTask* self, const BTUpdateContext* ctx)
{
  InitialPosition* initalPosition = ecs_get(ctx->registry, ctx->entity, INITIAL_POSITION);
  if (initalPosition != NULL)
  {
    blackboard_set_entry_as_vec2(ctx->blackboard, self->bbResultKey, *initalPosition);
    return BT_STATUS_SUCCESS;
  }
  return BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(GetInitialPositionTask, BTNode, bt_node, {
  vtbl->tick = (BTOnTickFunc)on_tick;
})


BTNode* bt_get_initial_position_task_new(u32 resultKey)
{
  GetInitialPositionTask* self = bt_alloc(vtbl_inst());

  return BT_NODE(init(self, resultKey));
}
