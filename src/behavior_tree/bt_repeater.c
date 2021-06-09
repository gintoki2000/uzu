#include "behavior_tree/bt_repeater.h"
#include "behavior_tree/bt_decorator_node.h"

BT_NODE_DECL(bt_Repeater, bt_DecoratorNode, { int total; })

BT_RUNTIME_DATA_DECL({
  bt_DecoratorNodeRuntimeData _base;
  int                         remaining;
})

#define super BT_DECORATOR_NODE(self)

static void on_start(bt_Repeater* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  runtimeData->remaining   = self->total;
}

static bt_Status on_tick(bt_Repeater* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, BT_NODE(self));
  if (runtimeData->remaining > 0)
  {
    --runtimeData->remaining;
    return (bt_decorator_node_on_tick(super, ctx) == BT_STATUS_FAILURE) ? BT_STATUS_FAILURE
                                                                        : BT_STATUS_RUNNING;
  }
  return BT_STATUS_SUCCESS;
}

bt_DecoratorNode* bt_repeater_create(int total)
{
  /*allocate memory*/
  bt_Repeater* node = SDL_malloc(sizeof(bt_Repeater));

  /*call super class constructor*/
  bt_decorator_node_init(BT_DECORATOR_NODE(node));

  /*overide virtual methods*/
  BT_NODE(node)->tick  = (bt_OnTickFunc)on_tick;
  BT_NODE(node)->start = (bt_OnStartFunc)on_start;

  /*init*/
  node->total = total;
  return (bt_DecoratorNode*)node;
}
