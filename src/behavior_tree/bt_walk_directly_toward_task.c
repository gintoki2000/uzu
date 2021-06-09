#include "behavior_tree/bt_node.h"
#include "behavior_tree/bt_walk_directly_toward_task.h"
#include "components.h"
#include "resources.h"

BT_NODE_DECL(bt_WalkDirectlyTowardTask, bt_Node, {
  u32   destinationKey;
  float stopWithinDistance;
})

BT_RUNTIME_DATA_DECL({ bt_Status pendingStatus; })

static void walk_directly_toward_completed_callback(RuntimeData* runtimeData, BOOL result)
{
  runtimeData->pendingStatus = result ? BT_STATUS_SUCCESS : BT_STATUS_FAILURE;
}

static void on_start(bt_WalkDirectlyTowardTask* self, const bt_UpdateContext* ctx)
{
  Vec2         destination;
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  if (ecs_has(ctx->registry, ctx->entity, WALK_DIRECTLY_TOWARD))
  {
    runtimeData->pendingStatus = BT_STATUS_FAILURE;
    return;
  }
  if (blackboard_get_entry_as_vec2(ctx->blackboard, self->destinationKey, &destination))
  {
    ecs_set(ctx->registry,
            ctx->entity,
            WALK_DIRECTLY_TOWARD,
            &(WalkDirectlyToward){
                .destination        = destination,
                .stopWithinDistance = self->stopWithinDistance,
                .cbCompleted = CALLBACK_1(runtimeData, walk_directly_toward_completed_callback),
            });
    runtimeData->pendingStatus = BT_STATUS_RUNNING;
    return;
  }
  runtimeData->pendingStatus = BT_STATUS_FAILURE;
}

static bt_Status on_tick(bt_WalkDirectlyTowardTask* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  return runtimeData->pendingStatus;
}

static void on_finish(bt_WalkDirectlyTowardTask* self, SDL_UNUSED bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  if (runtimeData->pendingStatus == BT_STATUS_RUNNING)
    ecs_rmv(ctx->registry, ctx->entity, WALK_DIRECTLY_TOWARD);

  runtimeData->pendingStatus = BT_STATUS_FAILURE;
}

bt_Node* bt_walk_directly_toward_task_create(u32 destinationKey, float stopWithinDistance)
{
  /*allocate memory*/
  bt_WalkDirectlyTowardTask* self = SDL_malloc(sizeof(bt_WalkDirectlyTowardTask));

  /*chain init*/
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_TASK);

  /*overide virtual methods*/
  BT_NODE(self)->finish = (bt_OnFinishFunc)on_finish;
  BT_NODE(self)->start  = (bt_OnStartFunc)on_start;
  BT_NODE(self)->tick   = (bt_OnTickFunc)on_tick;

  /*init*/
  self->destinationKey = destinationKey;
  self->stopWithinDistance = stopWithinDistance;

  return (bt_Node*)self;
}
