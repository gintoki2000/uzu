#include "behavior_tree/base.h"

BT_INST_DECL_EMPTY(Inverter, BTDecorator)
BT_VTBL_DECL_EMPTY(Inverter, BTDecorator)

#define super BT_DECORATOR(self)

static Inverter* initialize(Inverter* self)
{
  bt_decorator_init(super);
  return self;
}

static BTStatus on_tick(Inverter* self, const BTUpdateContext* ctx)
{
  switch (bt_decorator_on_tick(super, ctx))
  {
  case BT_STATUS_SUCCESS:
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  }
}

BT_VTBL_INITIALIZER(Inverter, BTDecorator, bt_decorator, {
  BT_NODE_VTBL(vtbl)->tick = (BTOnTickFunc)on_tick;
})

BTDecorator* bt_inverter_new()
{
  return BT_DECORATOR(initialize(bt_alloc(vtbl_inst())));
}
