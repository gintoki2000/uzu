#include "behavior_tree/bt_root.h"
#include "behavior_tree/bt_node.h"

struct bt_Root
{
  bt_Node  _base;
  bt_Node* child;
};

BT_RUNTIME_DATA_DECL({ BOOL isChildRunning; })

static void fini(bt_Root* self)
{
  bt_node_free(self->child);
}

static bt_Status on_tick(bt_Root* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (!runtimeData->isChildRunning)
  {
    bt_node_start(self->child, ctx);
    runtimeData->isChildRunning = TRUE;
  }
  if (bt_node_tick(self->child, ctx) != BT_STATUS_RUNNING)
  {
    runtimeData->isChildRunning = FALSE;
    bt_node_finish(self->child, ctx);
  }
  return BT_STATUS_RUNNING;
}

static void on_finish(bt_Root* self, const bt_UpdateContext* ctx)
{
  RuntimeData* runtimeData = bt_get_runtime_data(ctx->runtimeDataManager, (bt_Node*)self);
  if (runtimeData->isChildRunning)
  {
    bt_node_finish(self->child, ctx);
    runtimeData->isChildRunning = FALSE;
  }
}

void bt_root_attach(bt_Root* self, bt_Node* child)
{
  ASSERT_MSG(child != NULL && child->parent == NULL, "NULL or already attached to other node");
  ASSERT_MSG(self->child == NULL, "Root Node can only have one child");
  child->parent = BT_NODE(self);
  self->child   = child;
}

bt_Node* bt_root_child(bt_Root* self)
{
  return self->child;
}

bt_Root* bt_root_create()
{
  /*allocate memory*/
  bt_Root* self = SDL_malloc(sizeof(bt_Root));

  /*init base*/
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_ROOT);

  /*overide virtual methods*/
  bt_Node* node = BT_NODE(self);
  node->fini    = (bt_FinalizeFunc)fini;
  node->finish  = (bt_OnFinishFunc)on_finish;
  node->tick    = (bt_OnTickFunc)on_tick;

  /*init*/
  self->child = NULL;
  return self;
}
