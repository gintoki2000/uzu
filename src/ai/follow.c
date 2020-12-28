#include "follow.h"
#include <behaviour_tree.h>
#include <components.h>
struct Follow
{
  BT_EXTEND_NODE(BTNode)
  float arrive_radius;
};

const BTNodeVtbl* follow_vtbl_inst();
static void        follow_vtbl_init(BTNodeVtbl* vtbl);
static Follow*     follow_init(Follow* self, float arrive_radius);
static void        follow_abort(Follow* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus   follow_exec(Follow* self, Ecs* ecs, ecs_entity_t entity);
static void follow_finish(Follow* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);

static Vec2 get_entity_position(Ecs* ecs, ecs_entity_t entity)
{
  Transform* transform = ecs_get(ecs, entity, TRANSFORM);
  return transform->pos;
}

BT_VTBL_INST_FN(BTNode, follow)
BT_ALLOC_FN(Follow, follow)

static void follow_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)follow_abort;
  vtbl->exec   = (bt_exec_fn_t)follow_exec;
  vtbl->finish = (bt_finish_fn_t)follow_finish;
}

static Follow* follow_init(Follow* self, float arrive_radius)
{
  bt_node_init((BTNode*)self);
  self->arrive_radius = arrive_radius;
  return self;
}

static void follow_abort(Follow* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;

  Motion* motion = ecs_get(ecs, entity, MOTION);
  motion->vel    = VEC2_ZERO;
  ecs_rmv(ecs, entity, FOLLOWING_TARGET);
}

static void follow_finish(Follow* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status)
{
  (void)self;
  Motion* motion;
  if (finish_status == BT_STATUS_SUCCESS)
  {
    motion      = ecs_get(ecs, entity, MOTION);
    motion->vel = VEC2_ZERO;
    ecs_rmv(ecs, entity, FOLLOWING_TARGET);
  }
}

/*
#define VEC_FMT_PATTERN "(%2.f, %2.f)"
#define VEC_FMT_VARS(__v) (__v).x, (__v).y
*/

// follow task chỉ nên gắn component cho entity và follow system sẽ lo chuyện còn lại

static BTStatus follow_exec(Follow* self, Ecs* ecs, ecs_entity_t entity)
{
  FollowingTarget* target;
  Transform*           transform;
  Motion*              motion;
  Vec2                 desired;
  Vec2                 target_pos;

  if ((target = ecs_get(ecs, entity, FOLLOWING_TARGET)) != NULL)
  {
    transform = ecs_get(ecs, entity, TRANSFORM);
    motion    = ecs_get(ecs, entity, MOTION);

    ASSERT(transform != NULL);
    ASSERT(motion != NULL);

    target_pos = get_entity_position(ecs, *target);
    desired    = vec2_sub(target_pos, transform->pos);
    float dist = vec2_normalize(&desired);
    if (dist < self->arrive_radius)
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

Follow* follow_new(float arrive_radius)
{
  return follow_init(follow_alloc(), arrive_radius);
}
