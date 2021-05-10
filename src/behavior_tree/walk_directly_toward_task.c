#include "behavior_tree/base.h"
#include "components.h"

typedef struct WalkDirectlyTowardTask
{
  BTNode   _base;
  int      bbDestinationKey;
  BTStatus pendingStatus;
} WalkDirectlyTowardTask;

typedef BTNodeVtbl WalkDirectlyTowardTaskVtbl;

static WalkDirectlyTowardTask* initialize(WalkDirectlyTowardTask* self, int bbDestinationKey)
{
  bt_node_init(BT_NODE(self));
  self->pendingStatus    = BT_STATUS_FAILURE;
  self->bbDestinationKey = bbDestinationKey;
  return self;
}

static void walk_directly_toward_completed_callback(WalkDirectlyTowardTask* self, BOOL result)
{
  self->pendingStatus = result ? BT_STATUS_SUCCESS : BT_STATUS_FAILURE;
}

static void on_start(WalkDirectlyTowardTask* self, const BTUpdateContext* ctx)
{
  Vec2 destination;
  if (ecs_has(ctx->registry, ctx->entity, WALK_DIRECTLY_TOWARD))
  {
    self->pendingStatus = BT_STATUS_FAILURE;
    return;
  }
  if (blackboard_get_entry_as_vec2(ctx->blackboard, self->bbDestinationKey, &destination))
  {
    ecs_set(ctx->registry,
            ctx->entity,
            WALK_DIRECTLY_TOWARD,
            &(WalkDirectlyToward){
                .destination = destination,
                .cbCompleted = CALLBACK_1(self, walk_directly_toward_completed_callback),
            });
    self->pendingStatus = BT_STATUS_RUNNING;
    return;
  }
  self->pendingStatus = BT_STATUS_FAILURE;
}

static BTStatus on_tick(WalkDirectlyTowardTask* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  return self->pendingStatus;
}

static void on_finish(WalkDirectlyTowardTask* self, SDL_UNUSED BTUpdateContext* ctx)
{
  if (self->pendingStatus == BT_STATUS_RUNNING)
    ecs_rmv(ctx->registry, ctx->entity, WALK_DIRECTLY_TOWARD);

  self->pendingStatus = BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(WalkDirectlyTowardTask, BTNode, bt_node, {
  vtbl->finish = (BTOnFinishFunc)on_finish;
  vtbl->start  = (BTOnStartFunc)on_start;
  vtbl->tick   = (BTOnTickFunc)on_tick;
})

BT_INST_ALLOC_FN(WalkDirectlyTowardTask, walk_directly_toward_task)

BTNode* bt_walk_directly_toward_new(int bbDestinationKey)
{
  return BT_NODE(initialize(walk_directly_toward_task_alloc(), bbDestinationKey));
}
