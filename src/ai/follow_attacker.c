#include "ai/follow_attacker.h"
#include "behaviour_tree.h"
#include "components.h"
#include "utils.h"

struct BTTask_FollowAttacker
{
  BT_EXTEND_NODE(BTNode)
  float radius;
  BOOL  is_running;
};

static const BTNodeVtbl*      __vtbl_inst();
static void                   __vtbl_init(BTNodeVtbl* vtbl);
static BTTask_FollowAttacker* __init(BTTask_FollowAttacker* self, float radius);
static void
                __finish(BTTask_FollowAttacker* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static void     __abort(BTTask_FollowAttacker* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus __exec(BTTask_FollowAttacker* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(BTTask_FollowAttacker, _)

BTTask_FollowAttacker* bt_task_follow_attacker_new(float radius)
{
  return __init(__alloc(), radius);
}

static void __vtbl_init(BTNodeVtbl* vtbl)
{

  bt_node_vtbl_init(vtbl);
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

static BTTask_FollowAttacker* __init(BTTask_FollowAttacker* self, float radius)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  self->radius     = radius;
  return self;
}

static void __finish(BTTask_FollowAttacker* self,
                     SDL_UNUSED Ecs*         ecs,
                     SDL_UNUSED ecs_entity_t entity,
                     SDL_UNUSED BTStatus     finish_status)
{
  self->is_running = FALSE;
}

static void __abort(BTTask_FollowAttacker* self, Ecs* ecs, ecs_entity_t entity)
{
  ecs_rmv(ecs, entity, FOLLOWING_TARGET);
  self->is_running                             = FALSE;
  ((Motion*)ecs_get(ecs, entity, MOTION))->vel = VEC2_ZERO;
}

static BTStatus __exec(BTTask_FollowAttacker* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->is_running)
  {
    return ecs_has(ecs, entity, FOLLOWING_TARGET) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
  }
  else
  {
    Attacker* attacker = ecs_get(ecs, entity, ATTACKER);
    if (attacker && ecs_is_valid(ecs, attacker->value))
    {
      FollowingTarget* target;
      if (get_distance_between_two_entities(ecs, attacker->value, entity) <= self->radius)
        return BT_STATUS_FAILURE;
      target           = ecs_add(ecs, entity, FOLLOWING_TARGET);
      target->entity   = attacker->value;
      target->radius   = self->radius;
      self->is_running = TRUE;
      return BT_STATUS_RUNNING;
    }
    else
      return BT_STATUS_FAILURE;
  }
}
