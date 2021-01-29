#include "set_dest_to_spot.h"
#include <behaviour_tree.h>
#include <components.h>

struct SetDestToSpot
{
  BT_EXTEND_NODE(BTNode)
};

const BTNodeVtbl*    set_dest_to_spot_vtbl_inst();
static void           set_dest_to_spot_vtbl_init(BTNodeVtbl* vtbl);
static SetDestToSpot* set_dest_to_spot_init(SetDestToSpot* self);
static BTStatus      set_dest_to_spot_exec(SetDestToSpot* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(BTNode, set_dest_to_spot)
BT_ALLOC_FN(SetDestToSpot, set_dest_to_spot)

static void set_dest_to_spot_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)set_dest_to_spot_exec;
}

static SetDestToSpot* set_dest_to_spot_init(SetDestToSpot* self)
{
  bt_node_init((BTNode*)self);
  return self;
}

static BTStatus set_dest_to_spot_exec(SetDestToSpot* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  Spot*                spot;
  Destination* dest;

  spot = ecs_get(ecs, entity, SPOT);

  if (spot == NULL)
    return BT_STATUS_FAILURE;

  dest = ecs_add(ecs, entity, DESTINATION);

  *dest = spot->position;

  return BT_STATUS_SUCCESS;
}

SetDestToSpot* set_dest_to_spot_new()
{
  return set_dest_to_spot_init(set_dest_to_spot_alloc());
}
