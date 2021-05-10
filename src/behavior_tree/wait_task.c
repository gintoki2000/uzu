#include "behavior_tree/base.h"
#include "components.h"

BT_VTBL_DECL_EMPTY(WaitTask, BTNode)
BT_INST_DECL(WaitTask, BTNode, {
  int duration;
  int remaining;
})

static WaitTask* initialize(WaitTask* self, int duration)
{
  bt_node_init((BTNode*)self);
  self->duration  = duration;
  self->remaining = 0;
  return self;
}

static void on_start(WaitTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  self->remaining = self->duration;
}

static BTStatus on_tick(WaitTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  --self->remaining;
  return self->remaining > 0 ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
}

BT_VTBL_INITIALIZER(WaitTask, BTNode, bt_node, {
  ((BTNodeVtbl*)vtbl)->start = (BTOnStartFunc)on_start;
  ((BTNodeVtbl*)vtbl)->tick  = (BTOnTickFunc)on_tick;
})

BT_INST_ALLOC_FN(WaitTask, wait_task)

BTNode* bt_task_wait_new(int duration)
{
  return BT_NODE(initialize(wait_task_alloc(), duration));
}
