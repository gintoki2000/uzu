#include "behavior_tree/base.h"

BT_INST_DECL(Selector, BTCompositeNode, {
  int     activeChildIndex;
  BOOL    lastChildExcutionCompleted;
  BTNode* activeChild;
})

BT_VTBL_DECL_EMPTY(Selector, BTCompositeNode)

#define super BT_COMPOSITE_NODE(self)
typedef BTUpdateContext UpdateContext;

static Selector* initialize(Selector* self)
{
  bt_composite_node_init(super);
  self->activeChildIndex = 0;
  return self;
}

static void on_start(Selector* self, const UpdateContext* ctx)
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

static BTStatus on_tick(Selector* self, const UpdateContext* ctx)
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
    bt_node_finish(self->activeChild, ctx);
    self->lastChildExcutionCompleted = TRUE;
    return next_child(self) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
  case BT_STATUS_SUCCESS:
    bt_node_finish(self->activeChild, ctx);
    self->lastChildExcutionCompleted = TRUE;
    return BT_STATUS_SUCCESS;
  }
}

static void on_finish(Selector* self, const UpdateContext* ctx)
{
  if (!self->lastChildExcutionCompleted && self->activeChild != NULL)
    bt_node_finish(self->activeChild, ctx);
}

BT_VTBL_INITIALIZER(Selector, BTCompositeNode, bt_composite_node, {
  ((BTNodeVtbl*)vtbl)->tick   = (BTOnTickFunc)on_tick;
  ((BTNodeVtbl*)vtbl)->start  = (BTOnStartFunc)on_start;
  ((BTNodeVtbl*)vtbl)->finish = (BTOnFinishFunc)on_finish;
})

BT_INST_ALLOC_FN(Selector, selector)

BTCompositeNode* bt_selector_new()
{
  return BT_COMPOSITE_NODE(initialize(selector_alloc()));
}
