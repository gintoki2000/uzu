#include "behavior_tree/base.h"
BT_GLOBAL_VTBL_INITIALIZER_IMPL(BTDecorator, bt_decorator, BTNode, bt_node, {
  ((BTNodeVtbl*)vtbl)->fini   = (BTFinalizeFunc)bt_decorator_fini;
  ((BTNodeVtbl*)vtbl)->finish = (BTOnFinishFunc)bt_decorator_on_finish;
  ((BTNodeVtbl*)vtbl)->tick   = (BTOnTickFunc)bt_decorator_on_tick;
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

BTStatus bt_decorator_on_tick(BTDecorator* self, const BTUpdateContext* ctx)
{
  BTStatus status;
  if (!self->isChildRunning)
  {
    self->isChildRunning = TRUE;
    bt_node_start(self->child, ctx);
  }
  status = bt_node_tick(self->child, ctx);
  if (status == BT_STATUS_FAILURE || status == BT_STATUS_SUCCESS)
  {
    self->isChildRunning = FALSE;
    bt_node_finish(self->child, ctx);
  }
  return status;
}

void bt_decorator_on_finish(BTDecorator* self, const BTUpdateContext* ctx)
{
  if (self->isChildRunning)
    bt_node_finish(self->child, ctx);
}
