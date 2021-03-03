#include "bttask.h"

struct BTTask_Wait
{
  BT_EXTEND_NODE(BTNode)
  int total_ticks;
  int remaining;
};

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);

#define TASK BTTask_Wait

static TASK*    __init(TASK* self, int total_ticks);
static void     __abort(TASK* self, Ecs* ecs, ecs_entity_t entity);
static void     __finish(TASK* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static BTStatus __exec(TASK* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(TASK, _)

void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

TASK* bt_task_wait_new(int total_ticks)
{
  return __init(__alloc(), total_ticks);
}

TASK* __init(TASK* self, int total_ticks)
{
  bt_node_init((BTNode*)self);
  self->total_ticks = total_ticks;
  self->remaining   = total_ticks;
  return self;
}

void __finish(TASK*      self,
              SDL_UNUSED Ecs*         ecs,
              SDL_UNUSED ecs_entity_t entity,
              SDL_UNUSED BTStatus     finish_status)
{
  self->remaining = self->total_ticks;
}

void __abort(TASK* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  self->remaining = self->total_ticks;
}

BTStatus __exec(TASK* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  if (self->remaining > 0 && --self->remaining == 0)
  {
    return BT_STATUS_SUCCESS;
  }
  else
    return BT_STATUS_RUNNING;
}

#undef TASK
