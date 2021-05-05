#include "behaviour_tree.h"

typedef struct _BTRoot
{
  BTNode  _base;
  BTNode* child;
} BTRoot;

typedef struct _BTRootVtbl
{
  BTNodeVtbl _base;
} BTRootVtbl;

BT_PRIVATE_NODE(BTRoot)

static BTRoot* Init(BTRoot* self, BTNode* child)
{
  bt_node_init((BTNode*)self);
  self->child = child;
  return self;
}

static void Finalize(BTRoot* self)
{
  bt_node_del(self->child);
}

static BTStatus Execute(BTRoot* self, Ecs* ecs, ecs_entity_t entity)
{
  BTStatus finish_status = bt_node_exec(self->child, ecs, entity);
  if (finish_status != BT_STATUS_RUNNING)
    bt_node_finish(self->child, ecs, entity, finish_status == BT_STATUS_SUCCESS);
  return BT_STATUS_RUNNING;
}

static void Abort(BTRoot* self, Ecs* registry, ecs_entity_t entity)
{
  bt_node_abort(self->child, registry, entity);
}

static void VtblInit(BTRootVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->exec  = (BTExecuteFunc)Execute;
  ((BTNodeVtbl*)vtbl)->fini  = (BTFinalizeFunc)Finalize;
  ((BTNodeVtbl*)vtbl)->abort = (BTAbortFunc)Abort;
}

BTRoot* bt_root_new(BTNode* child)
{
  return Init(Alloc(), child);
}
