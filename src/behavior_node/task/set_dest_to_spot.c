#include "bttask.h"
#include <behaviour_tree.h>
#include <components.h>

struct BTTask_SetDestToSpot
{
  BT_EXTEND_NODE(BTNode)
};

#define TASK BTTask_SetDestToSpot

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);
static TASK*             __init(TASK* self);
static BTStatus          __exec(TASK* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(TASK, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
}

static TASK* __init(TASK* self)
{
  bt_node_init((BTNode*)self);
  return self;
}

static BTStatus __exec(SDL_UNUSED TASK* self, Ecs* ecs, ecs_entity_t entity)
{
  Spot*        spot;
  Destination* dest;

  spot = ecs_get(ecs, entity, SPOT);

  if (spot == NULL)
    return BT_STATUS_FAILURE;

  dest = ecs_add_w_data(ecs, entity, DESTINATION, &spot->position);

  return BT_STATUS_SUCCESS;
}

TASK* bt_task_set_dest_to_spot_new()
{
  return __init(__alloc());
}
