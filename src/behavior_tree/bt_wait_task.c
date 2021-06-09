#include "behavior_tree/bt_node.h"
#include "components.h"
#include "resources.h"

// declare class here
BT_NODE_DECL(bt_WaitTask, bt_Node, { u32 duration; })

// declare runtime data here
BT_RUNTIME_DATA_DECL({ u32 remaining; })

static void on_start(bt_WaitTask* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  runtimeData->remaining   = self->duration;
}

static bt_Status on_tick(bt_WaitTask* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  --runtimeData->remaining;
  return runtimeData->remaining > 0 ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
}

bt_Node* bt_wait_task_create(u32 duration)
{
  /*allocate memory*/
  bt_WaitTask* self = SDL_malloc(sizeof(bt_WaitTask));

  /*init base*/
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_TASK);

  /*overide methods*/
  BT_NODE(self)->start = (bt_OnStartFunc)on_start;
  BT_NODE(self)->tick  = (bt_OnTickFunc)on_tick;

  /*init*/
  self->duration = duration;
  return (bt_Node*)self;
}
