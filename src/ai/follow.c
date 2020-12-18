#include "follow.h"
#include <behaviour_tree.h>
#include <components.h>
struct Follow
{
  BT_EXTEND_NODE(bt_Node)
  float arrive_radius;
};

const bt_NodeVtbl* follow_vtbl_inst();
static void        follow_vtbl_init(bt_NodeVtbl* vtbl);
static Follow*     follow_init(Follow* self, float arrive_radius);
static void        follow_abort(Follow* self, Ecs* ecs, ecs_entity_t entity);
static bt_Status   follow_exec(Follow* self, Ecs* ecs, ecs_entity_t entity);
static void follow_finish(Follow* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);

static Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  Transform* transform = ecs_get(ecs, entity, TRANSFORM);
  return transform->pos;
}

BT_VTBL_INST_FN(bt_Node, follow)
BT_ALLOC_FN(Follow, follow)

static void follow_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)follow_abort;
  vtbl->exec   = (bt_exec_fn_t)follow_exec;
  vtbl->finish = (bt_finish_fn_t)follow_finish;
}

static Follow* follow_init(Follow* self, float arrive_radius)
{
  bt_node_init((bt_Node*)self);
  self->arrive_radius = arrive_radius;
  return self;
}

static void follow_abort(Follow* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  Motion* motion = ecs_get(ecs, entity, MOTION);
  motion->vel    = VEC2_ZERO;
  ecs_rmv(ecs, entity, BTV_FOLLOWING_TARGET);
}

static void follow_finish(Follow* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)self;
  Motion* motion;
  if (finish_status == BT_STATUS_SUCCESS)
  {
    motion      = ecs_get(ecs, entity, MOTION);
    motion->vel = VEC2_ZERO;
    ecs_rmv(ecs, entity, BTV_FOLLOWING_TARGET);
  }
}

/*
#define VEC_FMT_PATTERN "(%2.f, %2.f)"
#define VEC_FMT_VARS(__v) (__v).x, (__v).y
*/

static bt_Status follow_exec(Follow* self, Ecs* ecs, ecs_entity_t entity)
{
  btv_FollowingTarget* target;
  Transform*           transform;
  Motion*              motion;
  Vec2                 desired;
  Vec2                 target_pos;

  if ((target = ecs_get(ecs, entity, BTV_FOLLOWING_TARGET)) != NULL)
  {
    transform = ecs_get(ecs, entity, TRANSFORM);
    motion    = ecs_get(ecs, entity, MOTION);

    ASSERT(transform != NULL);
    ASSERT(motion != NULL);

    target_pos = get_entity_position(ecs, *target);
    desired    = subv(target_pos, transform->pos);
    float dist = normalizev(&desired);
    if (dist < self->arrive_radius)
    {
      return BT_STATUS_SUCCESS;
    }
    desired     = mulv(desired, motion->max_speed);
    motion->acc = truncatev(subv(desired, motion->vel), motion->max_force);
    return BT_STATUS_RUNNING;
  }
  else
  {
    return BT_STATUS_FAILURE;
  }
}

Follow* follow_new(float arrive_radius)
{
  return follow_init(follow_alloc(), arrive_radius);
}
