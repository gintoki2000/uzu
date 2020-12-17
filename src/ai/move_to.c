#include "move_to.h"
#include <components.h>
#include <constances.h>

#define SLOW_DOWN_RADIUS 32.f
#define ARRIVE_RADIUS 1.f

const bt_NodeVtbl* move_to_vtbl_inst();
void               move_to_vtbl_init(bt_NodeVtbl* vtbl);
MoveTo*            move_to_init(MoveTo* self, float slow_down_radius, float arrive_radius);
bt_Status          move_to_exec(MoveTo* self, Ecs* ecs, ecs_entity_t entity);

BT_ALLOC_FN(MoveTo, move_to)
BT_NODE_VTBL_INST_FN(bt_Node, move_to)

void move_to_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init((bt_NodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)move_to_exec;
}

MoveTo* move_to_new(float slow_down_radius, float arrive_radius)
{
  return move_to_init(move_to_alloc(), slow_down_radius, arrive_radius);
}

MoveTo* move_to_init(MoveTo* self, float slow_down_radius, float arrive_radius)
{
  bt_node_init((bt_Node*)self);
  self->arrive_radius    = arrive_radius;
  self->slow_down_radius = slow_down_radius;
  return self;
}

bt_Status move_to_exec(MoveTo* self, Ecs* ecs, ecs_entity_t entity)
{
  btv_MoveDestination* destination;
  Transform*           transform;
  Motion*              motion;
  Vec2                 desired;

  if ((destination = ecs_get(ecs, entity, BTV_MOVE_DESTINATION)) &&
      (transform = ecs_get(ecs, entity, TRANSFORM)) && (motion = ecs_get(ecs, entity, MOTION)))
  {
    desired    = subv(*destination, transform->pos);
    float dist = normalizev(&desired);
    if (dist < self->slow_down_radius)
    {
      if (dist < self->arrive_radius)
      {
        ecs_rmv(ecs, entity, BTV_MOVE_DESTINATION);
        motion->vel = VEC2_ZERO;
        return BT_STATUS_SUCCESS;
      }
      float desired_speed = motion->max_speed * (dist / self->slow_down_radius);
      desired             = mulv(desired, desired_speed);
    }
    else
    {
      desired = mulv(desired, motion->max_speed);
    }
    motion->acc = truncatev(subv(desired, motion->vel), motion->max_force);
    return BT_STATUS_RUNNING;
  }
  else
  {
    return BT_STATUS_FAILURE;
  }
}
