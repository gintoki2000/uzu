#include "behavior_tree/base.h"

BT_INST_DECL(Repeater, BTDecorator, {
  int total;
  int remaining;
})
BT_VTBL_DECL_EMPTY(Repeater, BTDecorator)
BT_INST_ALLOC_FN(Repeater, repeater)

#define super BT_DECORATOR(self)
static Repeater* initialize(Repeater* self, int total)
{
  bt_decorator_init((BTDecorator*)self);
  self->total     = total;
  self->remaining = 0;
  return self;
}

static void on_start(Repeater* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  self->remaining = self->total;
}

static BTStatus on_tick(Repeater* self, const BTUpdateContext* ctx)
{
  if (self->remaining > 0)
  {
    --self->remaining;
    return (bt_decorator_on_tick(super, ctx) == BT_STATUS_FAILURE) ? BT_STATUS_FAILURE
                                                                   : BT_STATUS_RUNNING;
  }
  return BT_STATUS_SUCCESS;
}

BT_VTBL_INITIALIZER(Repeater, BTDecorator, bt_decorator, {
  ((BTNodeVtbl*)vtbl)->tick  = (BTOnTickFunc)on_tick;
  ((BTNodeVtbl*)vtbl)->start = (BTOnStartFunc)on_start;
})

BTDecorator* bt_repeater_new(int totals)
{
  return BT_DECORATOR(initialize(repeater_alloc(), totals));
}
