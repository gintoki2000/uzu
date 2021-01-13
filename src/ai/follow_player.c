#include "follow_player.h"
#include <components.h>
#include <utils.h>
static const BTNodeVtbl*    __vtbl_inst();
static void                 __vtbl_init(BTNodeVtbl* vtbl);
static BTTask_FollowPlayer* __init(BTTask_FollowPlayer* self, float radius);
static void
                __finish(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
static void     __abort(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity);
static BTStatus __exec(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity);

BT_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(BTTask_FollowPlayer, _)

BTTask_FollowPlayer* bt_task_follow_player_new(float radius)
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

static BTTask_FollowPlayer* __init(BTTask_FollowPlayer* self, float radius)
{
  bt_node_init((BTNode*)self);
  self->is_running = FALSE;
  self->radius     = radius;
  return self;
}

static void
__finish(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->is_running = FALSE;
}

static void __abort(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity)
{

  ecs_rmv(ecs, entity, FOLLOWING_TARGET);
  self->is_running                             = FALSE;
  ((Motion*)ecs_get(ecs, entity, MOTION))->vel = VEC2_ZERO;
}

static BTStatus __exec(BTTask_FollowPlayer* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->is_running)
  {
    return (ecs_has(ecs, entity, FOLLOWING_TARGET)) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
  }
  else
  {
    ecs_entity_t player;
    if ((player = get_player(ecs)) != ECS_NULL_ENT)
    {
      Vec2             pos;
      Vec2             target_pos;
      FollowingTarget* target;
      pos        = get_entity_position(ecs, entity);
      target_pos = get_entity_position(ecs, player);
      if (vec2_mag(vec2_sub(target_pos, pos)) <= self->radius)
        return BT_STATUS_FAILURE;
      target           = ecs_add(ecs, entity, FOLLOWING_TARGET);
      target->entity   = player;
      target->radius   = self->radius;
      self->is_running = TRUE;
      return BT_STATUS_RUNNING;
    }
    else
      return BT_STATUS_FAILURE;
  }
}
