#include "behavior_tree/base.h"

BT_INST_DECL(Sequence, BTCompositeNode, {
  int     activeChildIndex;
  BOOL    lastChildExecutionCompleted;
  BTNode* activeChild;
})

BT_VTBL_DECL_EMPTY(Sequence, BTCompositeNode)

#define super BT_COMPOSITE_NODE(self)

static Sequence* init(Sequence* self)
{
  bt_composite_node_init(super);
  self->activeChildIndex            = -1;
  self->activeChild                 = NULL;
  self->lastChildExecutionCompleted = TRUE;
  return self;
}

static void on_start(Sequence* self, const BTUpdateContext* ctx)
{
  self->activeChildIndex            = 0;
  self->lastChildExecutionCompleted = FALSE;
  self->activeChild                 = bt_composite_node_child_at(super, 0);
  bt_node_start(self->activeChild, ctx);
}

static BOOL next_child(Sequence* self)
{
  if (self->activeChildIndex < super->children->cnt - 1)
  {
    self->activeChildIndex++;
    self->activeChild = bt_composite_node_child_at(super, self->activeChildIndex);
    return TRUE;
  }
  self->activeChild = NULL;
  return FALSE;
}

static BTStatus on_tick(Sequence* self, const BTUpdateContext* ctx)
{
  if (self->lastChildExecutionCompleted)
  {
    bt_node_start(self->activeChild, ctx);
    self->lastChildExecutionCompleted = FALSE;
  }
  switch (bt_node_tick(self->activeChild, ctx))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_finish(self->activeChild, ctx);
    self->lastChildExecutionCompleted = TRUE;
    return next_child(self) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(self->activeChild, ctx);
    self->lastChildExecutionCompleted = TRUE;
    return BT_STATUS_FAILURE;
  }
}

static void on_finish(Sequence* self, const BTUpdateContext* ctx)
{
  if (self->activeChild != NULL && !self->lastChildExecutionCompleted)
    bt_node_finish(self->activeChild, ctx);
  self->activeChildIndex            = -1;
  self->activeChild                 = NULL;
  self->lastChildExecutionCompleted = TRUE;
}

BT_VTBL_INITIALIZER(Sequence, BTCompositeNode, bt_composite_node, {
  ((BTNodeVtbl*)vtbl)->tick   = (BTOnTickFunc)on_tick;
  ((BTNodeVtbl*)vtbl)->start  = (BTOnStartFunc)on_start;
  ((BTNodeVtbl*)vtbl)->finish = (BTOnFinishFunc)on_finish;
})

BTCompositeNode* bt_sequence_new()
{
  Sequence* self = bt_alloc( vtbl_inst() );
  return BT_COMPOSITE_NODE(init(self));
}
