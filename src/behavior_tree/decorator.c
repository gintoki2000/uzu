#include "behavior_tree/base.h"
BT_GLOBAL_VTBL_INITIALIZER_IMPL(BTDecorator, bt_decorator, BTNode, bt_node, {
  BT_NODE_VTBL(vtbl)->fini   = (BTFinalizeFunc)bt_decorator_fini;
  BT_NODE_VTBL(vtbl)->finish = (BTOnFinishFunc)bt_decorator_on_finish;
  BT_NODE_VTBL(vtbl)->tick   = (BTOnTickFunc)bt_decorator_on_tick;
})

BTDecorator* bt_decorator_init(BTDecorator* self)
{
  bt_node_init(BT_NODE(self));
  self->child          = NULL;
  self->isChildRunning = FALSE;
  return self;
}

void bt_decorator_fini(BTDecorator* self)
{
  if (self->child != NULL)
  {
    bt_node_del(self->child);
    self->child = NULL;
  }
}

void bt_decorator_set_child(BTDecorator* self, BTNode* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  if (self->child != NULL)
    bt_node_del(self->child);
  self->child  = node;
  node->parent = BT_NODE(self);
}

BTNode* bt_decorator_steal_child(BTDecorator* self)
{
  BTNode* child = self->child;
  if (self->child != NULL)
  {
    self->child->parent = NULL;
    self->child         = NULL;
  }
  return child;
}

static void finish_child_execution(BTDecorator* self, const BTUpdateContext* ctx)
{
  self->isChildRunning = FALSE;
  bt_node_finish(self->child, ctx);
}

BTStatus bt_decorator_on_tick(BTDecorator* self, const BTUpdateContext* ctx)
{
  BTStatus status;
  if (!self->isChildRunning)
  {
    self->isChildRunning = TRUE;
    bt_node_start(self->child, ctx);
  }
  status = bt_node_tick(self->child, ctx);
  if (status != BT_STATUS_RUNNING)
  {
    finish_child_execution(self, ctx);
  }
  return status;
}

void bt_decorator_on_finish(BTDecorator* self, const BTUpdateContext* ctx)
{
  if (self->isChildRunning)
    finish_child_execution(self, ctx);
}
