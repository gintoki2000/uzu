#include "behavior_tree/bt_sequence.h"
#include "behavior_tree/bt_composite_node.h"

BT_NODE_DECL_EMPTY(bt_Sequence, bt_CompositeNode)

BT_RUNTIME_DATA_DECL({
  int      activeChildIndex;
  BOOL     lastChildExecutionCompleted;
  bt_Node* activeChild;
})

#define super BT_COMPOSITE_NODE(self)

static void on_start(bt_Sequence* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData      = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  runtimeData->activeChildIndex = 0;
  runtimeData->lastChildExecutionCompleted = FALSE;
  runtimeData->activeChild                 = bt_composite_node_child_at(super, 0);
  bt_node_start(runtimeData->activeChild, ctx);
}

static BOOL next_child(bt_Sequence* self, RuntimeData* runtimeData)
{
  if (runtimeData->activeChildIndex < super->children->count - 1)
  {
    runtimeData->activeChildIndex++;
    runtimeData->activeChild = bt_composite_node_child_at(super, runtimeData->activeChildIndex);
    return TRUE;
  }
  runtimeData->activeChild = NULL;
  return FALSE;
}

static bt_Status on_tick(bt_Sequence* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  if (runtimeData->lastChildExecutionCompleted)
  {
    bt_node_start(runtimeData->activeChild, ctx);
    runtimeData->lastChildExecutionCompleted = FALSE;
  }
  switch (bt_node_tick(runtimeData->activeChild, ctx))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_finish(runtimeData->activeChild, ctx);
    runtimeData->lastChildExecutionCompleted = TRUE;
    return next_child(self, runtimeData) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(runtimeData->activeChild, ctx);
    runtimeData->lastChildExecutionCompleted = TRUE;
    return BT_STATUS_FAILURE;
  }
}

static void on_finish(bt_Sequence* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  if (runtimeData->activeChild != NULL && !runtimeData->lastChildExecutionCompleted)
    bt_node_finish(runtimeData->activeChild, ctx);
  runtimeData->activeChildIndex            = -1;
  runtimeData->activeChild                 = NULL;
  runtimeData->lastChildExecutionCompleted = TRUE;
}

bt_CompositeNode* bt_sequence_create(void)
{
  /*allocate memory*/
  bt_Sequence* self = SDL_malloc(sizeof(bt_Sequence));

  /*call super class constructor*/
  bt_composite_node_init(BT_COMPOSITE_NODE(self));

  /*overide virtual methods*/
  BT_NODE(self)->tick   = (bt_OnTickFunc)on_tick;
  BT_NODE(self)->start  = (bt_OnStartFunc)on_start;
  BT_NODE(self)->finish = (bt_OnFinishFunc)on_finish;

  /*construct this object*/
  /*not thing*/

  return (bt_CompositeNode*)self;
}
