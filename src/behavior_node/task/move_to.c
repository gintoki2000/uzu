#include "bttask.h"
#include <components.h>
#include <constances.h>

struct BTTask_MoveTo
{
  BT_EXTEND_NODE(BTNode)
  float arrive_radius;
};

#define NODE BTTask_MoveTo

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);
static NODE*             __init(NODE*, float);
static BTStatus          __exec(NODE*, Ecs*, ecs_entity_t);
static void              __abort(NODE*, Ecs*, ecs_entity_t);
static void              __finish(NODE*, Ecs*, ecs_entity_t, BOOL);

BT_ALLOC_FN(BTTask_MoveTo, _)
BT_STATIC_VTBL_INST_FN(BTNode, _)

NODE* bt_task_move_to_new(float arrive_radius)
{
  return __init(__alloc(), arrive_radius);
}

void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
  vtbl->abort  = (bt_abort_fn_t)__abort;
}

static NODE* __init(NODE* self, float arrive_radius)
{
  bt_node_init((BTNode*)self);
  self->arrive_radius = arrive_radius;
  return self;
}

static BTStatus __exec(NODE* self, Ecs* ecs, ecs_entity_t entity)
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

static void __abort(SDL_UNUSED NODE* self, Ecs* ecs, ecs_entity_t entity)
{
  Motion* motion;
  if (ecs_has(ecs, entity, DESTINATION) && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}

static void __finish(SDL_UNUSED NODE* self, Ecs* ecs, ecs_entity_t entity, BOOL succeed)
{
  Motion* motion;
  if (succeed && (motion = ecs_get(ecs, entity, MOTION)))
  {
    ecs_rmv(ecs, entity, DESTINATION);
    motion->vel = VEC2_ZERO;
  }
}

#undef NODE
