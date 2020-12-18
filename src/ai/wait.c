#include "wait.h"

BT_VTBL_INST_FN(bt_Node, wait)
BT_ALLOC_FN(Wait, wait)

void wait_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)wait_abort;
  vtbl->exec   = (bt_exec_fn_t)wait_exec;
  vtbl->finish = (bt_finish_fn_t)wait_finish;
}

Wait* wait_new(int total_ticks)
{
  return wait_init(wait_alloc(), total_ticks);
}

Wait* wait_init(Wait* self, int total_ticks)
{
  bt_node_init((bt_Node*)self);
  self->total_ticks = total_ticks;
  self->remaining   = total_ticks;
  return self;
}

void wait_finish(Wait* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->remaining = self->total_ticks;
}

void wait_abort(Wait* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  self->remaining = self->total_ticks;
}

bt_Status wait_exec(Wait* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  if (--self->remaining == 0)
  {
    return BT_STATUS_SUCCESS;
  }
  else
    return BT_STATUS_RUNNING;
}
