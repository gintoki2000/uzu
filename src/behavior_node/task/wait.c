#include "bttask.h"
#include "components.h"

struct BTTask_Wait
{
  BT_EXTEND_NODE(BTNode)
  int  total_ticks;
  int  remaining;
  BOOL is_running;
};

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);

#define NODE BTTask_Wait

static NODE*    __init(NODE* self, int total_ticks);
static void     __abort(NODE* self, Ecs* ecs, ecs_entity_t entity);
static void     __finish(NODE* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static BTStatus __exec(NODE* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(NODE, _)

void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

NODE* bt_task_wait_new(int total_ticks)
{
  return __init(__alloc(), total_ticks);
}

NODE* __init(NODE* self, int total_ticks)
{
  bt_node_init((BTNode*)self);
  self->total_ticks = total_ticks;
  self->remaining   = total_ticks;
  self->is_running  = SDL_FALSE;
  return self;
}

void __finish(NODE*               self,
              Ecs*                ecs,
              ecs_entity_t        entity,
              SDL_UNUSED BTStatus finish_status /*success? -> BOOL*/)
{
  Controller* controller = ecs_get(ecs, entity, CONTROLLER);
  self->remaining        = self->total_ticks;
  self->is_running       = FALSE;
  controller->in_action  = FALSE;
}

void __abort(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  Controller* controller = ecs_get(ecs, entity, CONTROLLER);
  self->remaining        = self->total_ticks;
  self->is_running       = FALSE;
  controller->in_action  = FALSE;
}

BTStatus __exec(NODE* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  if (self->is_running)
  {
    if (self->remaining > 0 && --self->remaining == 0)
      return BT_STATUS_SUCCESS;
    else
      return BT_STATUS_RUNNING;
  }
  else
  {
    Controller* controller = ecs_get(ecs, entity, CONTROLLER);
    if (controller == NULL || controller->in_action)
      return BT_STATUS_FAILURE;
    self->is_running = TRUE;
    return BT_STATUS_RUNNING;
  }
}

#undef NODE
