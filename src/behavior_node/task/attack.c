
#include "bttask.h"
#include "components.h"
#include "constances.h"
#include "entity_utils.h"

struct BTTask_Attack
{
  BT_EXTEND_NODE(BTNode)
  BOOL is_running;
};

#define TASK BTTask_Attack

static void     __vtbl_init(BTNodeVtbl* vtbl);
static TASK*    __init(BTTask_Attack* self);
static void     __finish(TASK* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static void     __abort(TASK* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus __exec(TASK* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(TASK, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

TASK* bt_task_attack_new()
{
  return __init(__alloc());
}

static TASK* __init(BTTask_Attack* self)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  return self;
}

static void __abort(TASK* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  self->is_running = FALSE;
}

static void __finish(TASK*      self,
                     SDL_UNUSED Ecs*         ecs,
                     SDL_UNUSED ecs_entity_t entity,
                     SDL_UNUSED BTStatus     finish_status)
{
  self->is_running = FALSE;
}

static BTStatus __exec(TASK* self, Ecs* ecs, ecs_entity_t entity)
{
  Controller*   controller;
  Hand*         hand;
  AttackTarget* attack_target;
  BOOL          has_attack_target;

  controller    = ecs_get(ecs, entity, CONTROLLER);
  attack_target = ecs_get(ecs, entity, ATTACK_TARGET);

  has_attack_target = attack_target != NULL && attack_target->value != ECS_NULL_ENT;

  if (!self->is_running)
  {
    hand = ecs_get(ecs, entity, HAND);
    if (controller->in_action || ecs_has(ecs, entity, PARALYZED) || !has_attack_target ||
        hand->weapon == ECS_NULL_ENT)
      return BT_STATUS_FAILURE;

    controller->action = CHARACTER_ACTION_REGULAR_ATK;
    self->is_running   = TRUE;
    return BT_STATUS_RUNNING;
  }
  else
  {
    if (controller->in_action)
      return BT_STATUS_RUNNING;
    else
    {
      self->is_running = FALSE;
      return BT_STATUS_SUCCESS;
    }
  }
}

#undef TASK
