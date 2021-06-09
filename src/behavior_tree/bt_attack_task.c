#include "behavior_tree/bt_node.h"
#include "components.h"

BT_INST_DECL(bt_AttackTask, bt_Node, { u32 fireCode; })

BT_RUNTIME_DATA_DECL({ bt_Status pendingStatus; })

BT_VTBL_DECL_EMPTY(bt_AttackTask, bt_Node)

static bt_AttackTask* init(bt_AttackTask* self, u32 fireCode)
{
  self->fireCode = fireCode;
  return self;
}

static void attack_request_completed_callback(RuntimeData* runtimeData, BOOL result)
{
  runtimeData->pendingStatus = result ? BT_STATUS_SUCCESS : BT_STATUS_FAILURE;
}

static void on_start(bt_AttackTask* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData;

  runtimeData = bt_get_runtime_data(ctx->runtimeDataMgr, (bt_Node*)self);
  if (ecs_has(ctx->registry, ctx->entity, ATTACK_COMMAND))
  {
    runtimeData->pendingStatus = BT_STATUS_FAILURE;
  }
  else
  {
    runtimeData->pendingStatus = BT_STATUS_RUNNING;
    ecs_set(ctx->registry,
            ctx->entity,
            ATTACK_COMMAND,
            &(AttackCommand){
                .triggerCode = 0,
                .cbCompleted = CALLBACK_1(runtimeData, attack_request_completed_callback),
            });
  }
}

static bt_Status on_tick(bt_AttackTask* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataMgr, (bt_Node*)self);
  return runtimeData->pendingStatus;
}

static void on_finish(bt_AttackTask* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataMgr, (bt_Node*)self);
  if (runtimeData->pendingStatus == BT_STATUS_RUNNING)
  {
    ecs_rmv(ctx->registry, ctx->entity, ATTACK_COMMAND);
    runtimeData->pendingStatus = BT_STATUS_FAILURE;
  }
}

BT_VTBL_INITIALIZER(bt_AttackTask, bt_Node, bt_node, {
  BT_NODE_VTBL(vtbl)->start  = (bt_OnStartFunc)on_start;
  BT_NODE_VTBL(vtbl)->finish = (bt_OnFinishFunc)on_finish;
  BT_NODE_VTBL(vtbl)->tick   = (bt_OnTickFunc)on_tick;
})

bt_Node* bt_attack_task_new(u32 fireCode)
{
  bt_AttackTask* task = SDL_malloc(sizeof(bt_AttackTask));
  BT_NODE(task)->vtbl = vtbl_inst();
  return BT_NODE(init(task, fireCode));
}
