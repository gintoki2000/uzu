#include "attack.h"
#include <components.h>
#include <constances.h>

struct Attack
{
  BT_EXTEND_NODE(bt_Node)
  BOOL active;
};

static void      attack_vtbl_init(bt_NodeVtbl* vtbl);
static Attack*   attack_init(Attack* self);
static bt_Status attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity);

BT_NODE_VTBL_INST_FN(bt_Node, attack)
BT_ALLOC_FN(Attack, attack)

static void attack_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)attack_exec;
}

Attack* attack_new()
{
  return attack_init(attack_alloc());
}

static Attack* attack_init(Attack* self)
{
  bt_node_init((bt_Node*)self);
  self->active = FALSE;
  return self;
}

static bt_Status attack_exec(Attack* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  Controller* controller;
  INFO("\n");

  controller = ecs_get(ecs, entity, CONTROLLER);

  if (!self->active)
  {
    if (controller->in_action)
      return BT_STATUS_FAILURE;

    controller->action = CHARACTER_ACTION_REGULAR_ATK;
    self->active       = TRUE;
    return BT_STATUS_RUNNING;
  }
  else
  {
    if (controller->in_action)
      return BT_STATUS_RUNNING;
    else
    {
      self->active = FALSE;
      return BT_STATUS_SUCCESS;
    }
  }
}
