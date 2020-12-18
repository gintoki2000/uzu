#include "attack.h"
#include <components.h>
#include <constances.h>

struct Attack
{
  BT_EXTEND_NODE(bt_Node)
  BOOL is_running;
};

static void    attack_vtbl_init(bt_NodeVtbl* vtbl);
static Attack* attack_init(Attack* self);
static void    attack_finish(Attack* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);
static void    attack_abort(Attack* self, Ecs* ecs, ecs_entity_t entity);
static bt_Status attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(bt_Node, attack)
BT_ALLOC_FN(Attack, attack)

static void attack_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)attack_exec;
  vtbl->abort  = (bt_abort_fn_t)attack_abort;
  vtbl->finish = (bt_finish_fn_t)attack_finish;
}

Attack* attack_new()
{
  return attack_init(attack_alloc());
}

static Attack* attack_init(Attack* self)
{
  bt_node_init((bt_Node*)self);
  self->is_running = FALSE;
  return self;
}

static void attack_abort(Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  self->is_running = FALSE;
}

static void attack_finish(Attack* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->is_running = FALSE;
}

static bt_Status attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  Controller* controller;

  controller = ecs_get(ecs, entity, CONTROLLER);

  if (!self->is_running)
  {
    if (controller->in_action)
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
