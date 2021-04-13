#include "bttask.h"
#include "components.h"
struct BTTask_FollowAttackTarget
{
  BT_EXTEND_NODE(BTNode)
  float distance;
  BOOL  is_running;
};

static const BTNodeVtbl* __vtbl_inst();
static void              __vtbl_init(BTNodeVtbl* vtbl);

#define NODE BTTask_FollowAttackTarget

static NODE*    __init(NODE*, float);
static void     __abort(NODE*, Ecs*, ecs_entity_t);
static void     __finish(NODE*, Ecs*, ecs_entity_t, BOOL);
static BTStatus __exec(NODE*, Ecs*, ecs_entity_t);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(NODE, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

NODE* bt_task_follow_attack_target_new(float distance)
{
  return __init(__alloc(), distance);
}

static NODE* __init(NODE* self, float distance)
{
  bt_node_init((BTNode*)self);
  self->distance   = distance;
  self->is_running = FALSE;
  return self;
}

static void __finish(NODE*      self,
                     SDL_UNUSED Ecs*         registry,
                     SDL_UNUSED ecs_entity_t entity,
                     SDL_UNUSED BOOL         succeed)
{
  self->is_running = FALSE;
}

static void __abort(NODE* self, Ecs* registry, ecs_entity_t entity)
{
  self->is_running = FALSE;
  ecs_rmv(registry, entity, FOLLOWING_TARGET);
}

static BTStatus __exec(NODE* self, Ecs* registry, ecs_entity_t entity)
{
  AttackTarget* attack_target;

  if (self->is_running)
  {
    return (ecs_has(registry, entity, FOLLOWING_TARGET)) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
  }
  else if ((attack_target = ecs_get(registry, entity, ATTACK_TARGET)) != NULL)
  {
    self->is_running = TRUE;
    ecs_set(registry,
            entity,
            FOLLOWING_TARGET,
            &(FollowingTarget){ attack_target->value, .radius = self->distance });
    return BT_STATUS_RUNNING;
  }
  return BT_STATUS_FAILURE;
}
#undef NODE
