#include "btt_wait.h"

const static bt_NodeVtbl BTT_WAIT_VTBL = {
  .fini  = (destroy_fn_t)bt_node_fini,
  .start = (bt_start_fn_t)btt_wait_start,
  .exec  = (bt_exec_fn_t)btt_wait_exec,
};

btt_Wait* btt_wait_new(int ticks)
{
  btt_Wait* wait      = malloc(sizeof(btt_Wait));
  BT_NODE(wait)->vtbl = &BTT_WAIT_VTBL;
  return btt_wait_init(wait, ticks);
}

btt_Wait* btt_wait_init(btt_Wait* self, int ticks)
{
  bt_node_init((bt_Node*)self);
  self->initialTicks = ticks;
  self->ticks        = ticks;
  return self;
}

void btt_wait_start(btt_Wait* self)
{
  bt_node_start_impl(BT_NODE(self));
  self->ticks = self->initialTicks;
}

void btt_wait_exec(btt_Wait* self, Ecs* ecs, ecs_entity_t entity)
{
  //INFO("ticks: %d\n", self->ticks);
  (void)ecs;
  (void)entity;
  --self->ticks;
  if (self->ticks == 0)
    BT_NODE(self)->status = BT_STATUS_SUCCESS;
}
