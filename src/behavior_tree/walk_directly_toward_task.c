#include "behavior_tree/base.h"
#include "components.h"
#include "resources.h"
BT_INST_DECL(WalkDirectlyTowardTask, BTNode, {
  u32      bbDestinationKey;
  BTStatus pendingStatus;
})

BT_VTBL_DECL_EMPTY(WalkDirectlyTowardTask, BTNode)

static WalkDirectlyTowardTask* initialize(WalkDirectlyTowardTask* self, u32 destinationKey)
{
  bt_node_init(BT_NODE(self));
  self->pendingStatus    = BT_STATUS_FAILURE;
  self->bbDestinationKey = destinationKey;
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
  ecs_set(ctx->registry,
          ctx->entity,
          TEXT,
          &(Text){
              .value  = "walk_directly_toward",
              .font   = get_font(FONT_ITEM_PICKED_UP),
              .effect = { .alignment = FC_ALIGN_CENTER,
                          .scale     = { 1.f, 1.f },
                          .color     = { 0xff, 0xff, 0xff, 0xff } },
          });
  return self->pendingStatus;
}

static void on_finish(WalkDirectlyTowardTask* self, SDL_UNUSED BTUpdateContext* ctx)
{
  if (self->pendingStatus == BT_STATUS_RUNNING)
    ecs_rmv(ctx->registry, ctx->entity, WALK_DIRECTLY_TOWARD);

  self->pendingStatus = BT_STATUS_FAILURE;
}

BT_VTBL_INITIALIZER(WalkDirectlyTowardTask, BTNode, bt_node, {
  BT_NODE_VTBL(vtbl)->finish = (BTOnFinishFunc)on_finish;
  BT_NODE_VTBL(vtbl)->start  = (BTOnStartFunc)on_start;
  BT_NODE_VTBL(vtbl)->tick   = (BTOnTickFunc)on_tick;
})

#define ALLOC_MEM() bt_alloc(vtbl_inst())

BTNode* bt_walk_directly_toward_task_new(u32 destinationKey)
{
  return BT_NODE(initialize(ALLOC_MEM(), destinationKey));
}
