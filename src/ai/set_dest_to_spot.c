#include "set_dest_to_spot.h"
#include <behaviour_tree.h>
#include <components.h>

struct SetDestToSpot
{
  BT_EXTEND_NODE(bt_Node)
};

const bt_NodeVtbl*    set_dest_to_spot_vtbl_inst();
static void           set_dest_to_spot_vtbl_init(bt_NodeVtbl* vtbl);
static SetDestToSpot* set_dest_to_spot_init(SetDestToSpot* self);
static bt_Status      set_dest_to_spot_exec(SetDestToSpot* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(bt_Node, set_dest_to_spot)
BT_ALLOC_FN(SetDestToSpot, set_dest_to_spot)

static void set_dest_to_spot_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)set_dest_to_spot_exec;
}

static SetDestToSpot* set_dest_to_spot_init(SetDestToSpot* self)
{
  bt_node_init((bt_Node*)self);
  return self;
}

static bt_Status set_dest_to_spot_exec(SetDestToSpot* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  Spot*                spot;
  btv_MoveDestination* dest;

  spot = ecs_get(ecs, entity, SPOT);

  if (spot == NULL)
    return BT_STATUS_FAILURE;

  dest = ecs_add(ecs, entity, BTV_MOVE_DESTINATION);

  *dest = spot->pos;

  return BT_STATUS_SUCCESS;
}

SetDestToSpot* set_dest_to_spot_new()
{
  return set_dest_to_spot_init(set_dest_to_spot_alloc());
}
