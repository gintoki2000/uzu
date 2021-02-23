#include "ai/move_to.h"
#include <components.h>
#include <constances.h>

const BTNodeVtbl*     move_to_vtbl_inst();
static void           move_to_vtbl_init(BTNodeVtbl* vtbl);
static BTTask_MoveTo* move_to_init(BTTask_MoveTo* self, float arrive_radius);
static BTStatus       move_to_exec(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity);
static void           move_to_abort(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity);
static void
move_to_finish(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);

BT_ALLOC_FN(BTTask_MoveTo, move_to)
BT_VTBL_INST_FN(BTNode, move_to)

void move_to_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)move_to_exec;
  vtbl->finish = (bt_finish_fn_t)move_to_finish;
  vtbl->abort  = (bt_abort_fn_t)move_to_abort;
}

BTTask_MoveTo* bt_task_move_to_new(float arrive_radius)
{
  return move_to_init(move_to_alloc(), arrive_radius);
}

static BTTask_MoveTo* move_to_init(BTTask_MoveTo* self, float arrive_radius)
{
  bt_node_init((BTNode*)self);
  self->arrive_radius = arrive_radius;
  return self;
}

static BTStatus move_to_exec(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  Destination* destination;
  Transform*   transform;
  Vec2         desired;
  Controller*  controller;

  if ((destination = ecs_get(ecs, entity, DESTINATION)) &&
      (transform = ecs_get(ecs, entity, TRANSFORM)) &&
      (controller = ecs_get(ecs, entity, CONTROLLER)))
  {
    desired = vec2_sub(*destination, transform->position);
    float d = vec2_normalize(&desired);
    if (d < self->arrive_radius)
    {
      controller->desired_direction = VEC2_ZERO;
      return BT_STATUS_SUCCESS;
    }
    controller->desired_direction = desired;
    return BT_STATUS_RUNNING;
  }
  else
  {

    return BT_STATUS_FAILURE;
  }
}

static void move_to_abort(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;
  Motion* motion;

  if (ecs_has(ecs, entity, DESTINATION) && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}

static void
move_to_finish(BTTask_MoveTo* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status)
{

  (void)self;
  Motion* motion;

  if (finish_status == BT_STATUS_SUCCESS && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}
