#include "move_to.h"
#include <components.h>
#include <constances.h>

const bt_NodeVtbl* move_to_vtbl_inst();
static void        move_to_vtbl_init(bt_NodeVtbl* vtbl);
static MoveTo*     move_to_init(MoveTo* self, float arrive_radius);
static bt_Status   move_to_exec(MoveTo* self, Ecs* ecs, ecs_entity_t entity);
static void        move_to_abort(MoveTo* self, Ecs* ecs, ecs_entity_t entity);
static void move_to_finish(MoveTo* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);

BT_ALLOC_FN(MoveTo, move_to)
BT_VTBL_INST_FN(bt_Node, move_to)

void move_to_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init((bt_NodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)move_to_exec;
  vtbl->finish = (bt_finish_fn_t)move_to_finish;
  vtbl->abort  = (bt_abort_fn_t)move_to_abort;
}

MoveTo* move_to_new(float arrive_radius)
{
  return move_to_init(move_to_alloc(), arrive_radius);
}

static MoveTo* move_to_init(MoveTo* self, float arrive_radius)
{
  bt_node_init((bt_Node*)self);
  self->arrive_radius = arrive_radius;
  return self;
}

static bt_Status move_to_exec(MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  Destination* destination;
  Transform*           transform;
  Motion*              motion;
  Vec2                 desired;

  if ((destination = ecs_get(ecs, entity, DESTINATION)) &&
      (transform = ecs_get(ecs, entity, TRANSFORM)) && (motion = ecs_get(ecs, entity, MOTION)))
  {
    desired = vec2_sub(*destination, transform->pos);
    float d = vec2_normalize(&desired);
    if (d < self->arrive_radius)
    {
      return BT_STATUS_SUCCESS;
    }
    desired     = vec2_mul(desired, motion->max_speed);
    motion->acc = vec2_trunc(vec2_sub(desired, motion->vel), motion->max_force);
    return BT_STATUS_RUNNING;
  }
  else
  {
    return BT_STATUS_FAILURE;
  }
}

static void move_to_abort(MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;
  Motion* motion;

  if (ecs_has(ecs, entity, DESTINATION) && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}

static void move_to_finish(MoveTo* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{

  (void)self;
  Motion* motion;

  if (finish_status == BT_STATUS_SUCCESS && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}
