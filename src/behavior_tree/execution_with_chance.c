#include "behavior_tree/base.h"

BT_INST_DECL(ExecutionWithChance, BTDecorator, { int chance; })
typedef BTDecoratorVtbl ExecutionWithChanceVtbl;

#define super ((BTDecorator*)self)
static ExecutionWithChance* initialize(ExecutionWithChance* self, int chance)
{
  bt_decorator_init(super);
  ASSERT_MSG(chance > 0 && chance <= 100, "invalid argument");
  self->chance = chance;
  return self;
}

static BTStatus on_tick(ExecutionWithChance* self, const BTUpdateContext* ctx)
{
  if (super->isChildRunning || self->chance <= (rand() % 101))
    return bt_decorator_on_tick(super, ctx);
  else
    return BT_STATUS_FAILURE;
}
BT_VTBL_INITIALIZER(ExecutionWithChance, BTDecorator, bt_decorator, {
  BT_NODE_VTBL(vtbl)->tick = (BTOnTickFunc)on_tick;
})

BTNode* bt_execution_with_chance_new(int chance)
{
  return BT_NODE(initialize(bt_alloc (vtbl_inst()), chance));
}
