#include "behaviour_tree.h"

typedef struct
{
  BTCompositeNode _base;
  int             curr;
} BTSequence;

typedef struct _BTSequenceVtbl
{
  BTCompositeNodeVtbl _base;
} BTSequenceVtbl;

static BTSequence* Init(BTSequence* self);
static void        Abort(BTSequence* self, Ecs* registry, ecs_entity_t entity);
static BTStatus    Execute(BTSequence* self, Ecs* registry, ecs_entity_t entity);
static void        Finish(BTSequence* self, Ecs* registry, ecs_entity_t entity, BOOL succeed);

BT_PRIVATE_NODE(BTSequence)

#define SUPER BT_COMPOSITE_NODE(self)

static void VtblInit(BTSequenceVtbl* vtbl)
{
  bt_composite_node_vtbl_init((BTCompositeNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->abort  = (BTAbortFunc)Abort;
  ((BTNodeVtbl*)vtbl)->exec   = (BTExecuteFunc)Execute;
  ((BTNodeVtbl*)vtbl)->finish = (BTFinishFunc)Finish;
}

BTCompositeNode* bt_sequence_new()
{
  return BT_COMPOSITE_NODE(Init(Alloc()));
}

static BTSequence* Init(BTSequence* self)
{
  bt_node_init((BTNode*)self);
  self->curr = 0;
  return self;
}

static void Abort(BTSequence* self, Ecs* registry, ecs_entity_t entity)
{
  if (SUPER->children->cnt > 0)
    bt_node_abort(bt_composite_node_child_at(SUPER, self->curr), registry, entity);
  self->curr = 0;
}

static BOOL NextChild(BTSequence* self)
{
  if (self->curr < SUPER->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}

static BTStatus Execute(BTSequence* self, Ecs* registry, ecs_entity_t entity)
{
  BTNode* current_node = bt_composite_node_child_at(SUPER, self->curr);
  switch (bt_node_exec(current_node, registry, entity))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_finish(current_node, registry, entity, TRUE);
    return NextChild(self) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(current_node, registry, entity, FALSE);
    return BT_STATUS_FAILURE;
  }
}

static void Finish(BTSequence* self,
                   SDL_UNUSED Ecs*         registry,
                   SDL_UNUSED ecs_entity_t entity,
                   SDL_UNUSED BOOL         succeed)
{
  self->curr = 0;
}
