#include "behavior_tree/base.h"

BT_INST_DECL(Selector, BTCompositeNode, {
  int     activeChildIndex;
  BOOL    lastChildExcutionCompleted;
  BTNode* activeChild;
})

BT_VTBL_DECL_EMPTY(Selector, BTCompositeNode)

#define super BT_COMPOSITE_NODE(self)

static Selector* init(Selector* self)
{
  bt_composite_node_init(super);
  self->activeChildIndex = 0;
  return self;
}

static void on_start(Selector* self, const BTUpdateContext* ctx)
{
  if (super->children->cnt == 0)
    return;
  self->activeChildIndex           = 0;
  self->activeChild                = bt_composite_node_child_at(super, 0);
  self->lastChildExcutionCompleted = FALSE;
  bt_node_start(self->activeChild, ctx);
}

static BOOL next_child(Selector* self)
{
  if (self->activeChildIndex < super->children->cnt - 1)
  {
    self->activeChildIndex++;
    self->activeChild = bt_composite_node_child_at(super, self->activeChildIndex);
    return TRUE;
  }
  self->activeChildIndex = -1;
  self->activeChild      = NULL;
  return FALSE;
}

static void finish_child_execution(Selector* self, const BTUpdateContext* ctx)
{
  self->lastChildExcutionCompleted = TRUE;
  bt_node_finish(self->activeChild, ctx);
}

static BTStatus on_tick(Selector* self, const BTUpdateContext* ctx)
{
  if (self->lastChildExcutionCompleted)
  {
    bt_node_start(self->activeChild, ctx);
    self->lastChildExcutionCompleted = FALSE;
  }
  switch (bt_node_tick(self->activeChild, ctx))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  case BT_STATUS_FAILURE:
    finish_child_execution(self, ctx);
    return next_child(self) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
  case BT_STATUS_SUCCESS:
    finish_child_execution(self, ctx);
    return BT_STATUS_SUCCESS;
  }
}

static void on_finish(Selector* self, const BTUpdateContext* ctx)
{
  if (!self->lastChildExcutionCompleted && self->activeChild != NULL)
    bt_node_finish(self->activeChild, ctx);
}

BT_VTBL_INITIALIZER(Selector, BTCompositeNode, bt_composite_node, {
  BT_NODE_VTBL(vtbl)->tick   = (BTOnTickFunc)on_tick;
  BT_NODE_VTBL(vtbl)->start  = (BTOnStartFunc)on_start;
  BT_NODE_VTBL(vtbl)->finish = (BTOnFinishFunc)on_finish;
})

BTCompositeNode* bt_selector_new()
{
  Selector* self = bt_alloc(vtbl_inst());
  return BT_COMPOSITE_NODE(init(self));
}
