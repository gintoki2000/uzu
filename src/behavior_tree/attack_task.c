#include "behavior_tree/base.h"
#include "components.h"

typedef struct
{
  BTNode   _base;
  BTStatus pendingStatus;
  u32      fireCode;
} AttackTask;

typedef struct
{
  BTNodeVtbl _base;
} AttackTaskVtbl;

static AttackTask* init(AttackTask* self, u32 fireCode)
{
  self->pendingStatus = BT_STATUS_FAILURE;
  self->fireCode      = fireCode;
  return self;
}

static void attack_request_completed_callback(AttackTask* self, BOOL result)
{
  self->pendingStatus = result ? BT_STATUS_SUCCESS : BT_STATUS_FAILURE;
}

static void on_start(AttackTask* self, const BTUpdateContext* ctx)
{
  if (ecs_has(ctx->registry, ctx->entity, ATTACK_COMMAND))
  {
    self->pendingStatus = BT_STATUS_FAILURE;
  }
  else
  {
    self->pendingStatus = BT_STATUS_RUNNING;
    ecs_set(ctx->registry,
            ctx->entity,
            ATTACK_COMMAND,
            &(AttackCommand){
                .code        = 0,
                .cbCompleted = CALLBACK_1(self, attack_request_completed_callback),
            });
  }
}

static BTStatus on_tick(AttackTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  return self->pendingStatus;
}

static void on_finish(AttackTask* self, const BTUpdateContext* ctx)
{
  if (self->pendingStatus == BT_STATUS_RUNNING)
  {
    ecs_rmv(ctx->registry, ctx->entity, ATTACK_COMMAND);
    self->pendingStatus = BT_STATUS_FAILURE;
  }
}

BT_VTBL_INITIALIZER(AttackTask, BTNode, bt_node, {
  BT_NODE_VTBL(vtbl)->start  = (BTOnStartFunc)on_start;
  BT_NODE_VTBL(vtbl)->finish = (BTOnFinishFunc)on_finish;
  BT_NODE_VTBL(vtbl)->tick   = (BTOnTickFunc)on_tick;
})

BTNode* bt_attack_task_new(u32 fireCode)
{
  return BT_NODE(init(bt_alloc(vtbl_inst()), fireCode));
}
