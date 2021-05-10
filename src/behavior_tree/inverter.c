#include "behavior_tree/base.h"

BT_INST_DECL(Inverter, BTDecorator, { BOOL is_child_running; })
BT_VTBL_DECL_EMPTY(Inverter, BTDecorator)

#define super BT_DECORATOR(self)

static Inverter* initialize(Inverter* self)
{
  bt_decorator_init(super);
  self->is_child_running = FALSE;
  return self;
}

static void on_start(Inverter* self, const BTUpdateContext* ctx)
{
  self->is_child_running = TRUE;
  bt_node_start(super->child, ctx);
}

static void on_finish(Inverter* self, const BTUpdateContext* ctx)
{
  if (self->is_child_running)
    bt_node_finish(self, ctx);
}

static BTStatus on_tick(Inverter* self, const BTUpdateContext* ctx)
{
  switch (bt_node_tick(super->child, ctx))
  {
  case BT_STATUS_SUCCESS:
    bt_node_finish(super->child, ctx);
    self->is_child_running = FALSE;
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    self->is_child_running = FALSE;
    bt_node_finish(super->child, ctx);
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    self->is_child_running = TRUE;
    return BT_STATUS_RUNNING;
  }
}

BT_VTBL_INITIALIZER(Inverter, BTDecorator, bt_decorator, {
  ((BTNodeVtbl*)vtbl)->finish = (BTOnFinishFunc)on_finish;
  ((BTNodeVtbl*)vtbl)->tick   = (BTOnTickFunc)on_tick;
  ((BTNodeVtbl*)vtbl)->start  = (BTOnStartFunc)on_start;
})

BT_INST_ALLOC_FN(Inverter, inverter)
BTDecorator* bt_inverter_new()
{
  return BT_DECORATOR(initialize(inverter_alloc()));
}
