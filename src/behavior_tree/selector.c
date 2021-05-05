#include "behaviour_tree.h"

typedef struct _BTSelector
{
  BTCompositeNode _base;
  int             curr;
} BTSelector;

typedef struct _BTSelectorVtbl
{
  BTCompositeNodeVtbl _base;
} BTSelectorVtbl;

#define super BT_COMPOSITE_NODE(self)

BT_PRIVATE_NODE(BTSelector)

static BTSelector* Init(BTSelector* self);
static void        Abort(BTSelector* self, Ecs* registry, ecs_entity_t entity);
static BTStatus    Execute(BTSelector* self, Ecs* registry, ecs_entity_t entity);
static void        Finish(BTSelector* self, Ecs* registry, ecs_entity_t entity, BOOL succeed);

static void VtblInit(BTSelectorVtbl* vtbl)
{
  bt_composite_node_vtbl_init((BTCompositeNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->exec   = (BTExecuteFunc)Execute;
  ((BTNodeVtbl*)vtbl)->abort  = (BTAbortFunc)Abort;
  ((BTNodeVtbl*)vtbl)->finish = (BTFinishFunc)Finish;
}

BTCompositeNode* bt_selector_new()
{
  return BT_COMPOSITE_NODE(Init(Alloc()));
}

static BTSelector* Init(BTSelector* self)
{
  bt_composite_node_init(super);
  self->curr = 0;
  return self;
}

static void Abort(BTSelector* self, Ecs* registry, ecs_entity_t entity)
{
  if (super->children->cnt > 0)
    bt_node_abort(bt_composite_node_child_at(super, self->curr), registry, entity);
  self->curr = 0;
}

static BOOL NextChild(BTSelector* self)
{
  if (self->curr < super->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}

static BTStatus Execute(BTSelector* self, Ecs* registry, ecs_entity_t entity)
{
  BTStatus finish_status;
  BTNode*  current_node;
  current_node  = bt_composite_node_child_at(super, self->curr);
  finish_status = bt_node_exec(current_node, registry, entity);
  switch (finish_status)
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  case BT_STATUS_FAILURE:
    bt_node_finish(current_node, registry, entity, FALSE);
    return NextChild(self) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
  case BT_STATUS_SUCCESS:
    bt_node_finish(current_node, registry, entity, TRUE);
    return BT_STATUS_SUCCESS;
  }
}

static void Finish(BTSelector* self,
                   SDL_UNUSED Ecs*         registry,
                   SDL_UNUSED ecs_entity_t entity,
                   SDL_UNUSED BOOL         succeed)
{
  self->curr = 0;
}
