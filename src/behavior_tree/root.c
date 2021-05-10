#include "behavior_tree/base.h"

typedef struct _BTRoot
{
  BTNode  _base;
  BTNode* child;
  BOOL    isChildRunning;
} BTRoot;

typedef BTNode BTRootVtbl;

static BTRoot* initialize(BTRoot* self, BTNode* child)
{
  bt_node_init((BTNode*)self);
  self->child          = child;
  self->isChildRunning = FALSE;
  return self;
}

static void finalize(BTRoot* self)
{
  bt_node_del(self->child);
}

static BTStatus on_tick(BTRoot* self, const BTUpdateContext* ctx)
{
  if (!self->isChildRunning)
  {
    bt_node_start(self->child, ctx);
    self->isChildRunning = TRUE;
  }
  if (bt_node_tick(self->child, ctx) != BT_STATUS_RUNNING)
  {
    self->isChildRunning = FALSE;
    bt_node_finish(self->child, ctx);
  }
  return BT_STATUS_RUNNING;
}

static void on_finish(BTRoot* self, const BTUpdateContext* ctx)
{
  if (self->isChildRunning)
  {
    bt_node_finish(self->child, ctx);
    self->isChildRunning = FALSE;
  }
}

BT_VTBL_INITIALIZER(BTRoot, BTNode, bt_node, {
  ((BTNodeVtbl*)vtbl)->fini   = (BTFinalizeFunc)finalize;
  ((BTNodeVtbl*)vtbl)->finish = (BTOnFinishFunc)on_finish;
  ((BTNodeVtbl*)vtbl)->tick   = (BTOnTickFunc)on_tick;
})

BT_INST_ALLOC_FN(BTRoot, root)

BTRoot* bt_root_new(BTNode* child)
{
  return initialize(root_alloc(), child);
}
