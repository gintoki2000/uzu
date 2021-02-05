#include "ai/wait.h"

BT_VTBL_INST_FN(BTNode, wait)
BT_ALLOC_FN(BTTask_Wait, wait)

void wait_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)wait_abort;
  vtbl->exec   = (bt_exec_fn_t)wait_exec;
  vtbl->finish = (bt_finish_fn_t)wait_finish;
}

BTTask_Wait* bt_task_wait_new(int total_ticks)
{
  return wait_init(wait_alloc(), total_ticks);
}

BTTask_Wait* wait_init(BTTask_Wait* self, int total_ticks)
{
  bt_node_init((BTNode*)self);
  self->total_ticks = total_ticks;
  self->remaining   = total_ticks;
  return self;
}

void wait_finish(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->remaining = self->total_ticks;
}

void wait_abort(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  self->remaining = self->total_ticks;
}

BTStatus wait_exec(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity)
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
