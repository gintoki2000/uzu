#include "behavior_tree/bt_composite_node.h"
#include "behavior_tree/bt_node.h"

BT_NODE_DECL_EMPTY(bt_Selector, bt_CompositeNode)

BT_RUNTIME_DATA_DECL({
  int      activeChildIndex;
  BOOL     lastChildExcutionCompleted;
  bt_Node* activeChild;
})

#define super BT_COMPOSITE_NODE(self)

static void on_start(bt_Selector* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (super->children->count == 0)
    return;
  runtimeData->activeChildIndex           = 0;
  runtimeData->activeChild                = bt_composite_node_child_at(super, 0);
  runtimeData->lastChildExcutionCompleted = FALSE;
  bt_node_start(runtimeData->activeChild, ctx);
}

static BOOL next_child(bt_Selector* self, RuntimeData* runtimeData)
{
  if (runtimeData->activeChildIndex < super->children->count - 1)
  {
    runtimeData->activeChildIndex++;
    runtimeData->activeChild = bt_composite_node_child_at(super, runtimeData->activeChildIndex);
    return TRUE;
  }
  runtimeData->activeChildIndex = -1;
  runtimeData->activeChild      = NULL;
  return FALSE;
}

static void finish_child_execution(bt_Selector* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  runtimeData->lastChildExcutionCompleted = TRUE;
  bt_node_finish(runtimeData->activeChild, ctx);
}

static bt_Status on_tick(bt_Selector* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (runtimeData->lastChildExcutionCompleted)
  {
    bt_node_start(runtimeData->activeChild, ctx);
    runtimeData->lastChildExcutionCompleted = FALSE;
  }
  switch (bt_node_tick(runtimeData->activeChild, ctx))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  case BT_STATUS_FAILURE:
    finish_child_execution(self, ctx);
    return next_child(self, runtimeData) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
  case BT_STATUS_SUCCESS:
    finish_child_execution(self, ctx);
    return BT_STATUS_SUCCESS;
  }
}

static void on_finish(bt_Selector* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (!runtimeData->lastChildExcutionCompleted && runtimeData->activeChild != NULL)
    bt_node_finish(runtimeData->activeChild, ctx);
}

bt_CompositeNode* bt_selector_create()
{
  /*allocate memory*/
  bt_Selector* self = SDL_malloc(sizeof(bt_Selector));

  /*chain construction*/
  bt_composite_node_init(super);

  /*overide virtual methods*/
  BT_NODE(self)->tick   = (bt_OnTickFunc)on_tick;
  BT_NODE(self)->start  = (bt_OnStartFunc)on_start;
  BT_NODE(self)->finish = (bt_OnFinishFunc)on_finish;

  /*init*/


  return (bt_CompositeNode*)self;
}
