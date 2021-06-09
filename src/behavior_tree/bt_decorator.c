#include "behavior_tree/bt_decorator_node.h"

bt_DecoratorNode* bt_decorator_node_init(bt_DecoratorNode* self)
{
  /*call super class constructor*/
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_COMPOSITE);

  /*overide virtual methods*/
  bt_Node* node = BT_NODE(self);
  node->fini    = (bt_FinalizeFunc)bt_decorator_node_fini;
  node->finish  = (bt_OnFinishFunc)bt_decorator_node_on_finish;
  node->tick    = (bt_OnTickFunc)bt_decorator_node_on_tick;
  node->type    = BT_NODE_TYPE_DECORATOR;

  /*construct this object*/
  self->child = NULL;
  return self;
}

void bt_decorator_node_fini(bt_DecoratorNode* self)
{
  if (self->child != NULL)
  {
    bt_node_free(self->child);
    self->child = NULL;
  }
}

void bt_decorator_node_attach(bt_DecoratorNode* self, bt_Node* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  if (self->child != NULL)
    bt_node_free(self->child);
  self->child  = node;
  node->parent = BT_NODE(self);
}

bt_Node* bt_decorator_node_steal(bt_DecoratorNode* self)
{
  bt_Node* child = self->child;
  if (self->child != NULL)
  {
    self->child->parent = NULL;
    self->child         = NULL;
  }
  return child;
}

static void finish_child_execution(bt_DecoratorNode* self, const bt_UpdateContext* ctx)
{
  bt_DecoratorNodeRuntimeData* runtimeData;

  runtimeData                 = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  runtimeData->isChildRunning = FALSE;
  bt_node_finish(self->child, ctx);
}

bt_Status bt_decorator_node_on_tick(bt_DecoratorNode* self, const bt_UpdateContext* ctx)
{
  bt_Status                    status;
  bt_DecoratorNodeRuntimeData* runtimeData;

  runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (!runtimeData->isChildRunning)
  {
    runtimeData->isChildRunning = TRUE;
    bt_node_start(self->child, ctx);
  }
  status = bt_node_tick(self->child, ctx);
  if (status != BT_STATUS_RUNNING)
  {
    finish_child_execution(self, ctx);
  }
  return status;
}

void bt_decorator_node_on_finish(bt_DecoratorNode* self, const bt_UpdateContext* ctx)
{
  bt_DecoratorNodeRuntimeData* runtimeData;

  runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (runtimeData->isChildRunning)
    finish_child_execution(self, ctx);
}
