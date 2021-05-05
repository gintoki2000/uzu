#include "behaviour_tree.h"

typedef struct
{
  BTDecorator _base;
  int         total;
  int         remaining;
} BTRepeater;

typedef struct
{
  BTDecoratorVtbl _base;
} BTRepeaterVtbl;
BT_PRIVATE_NODE(BTRepeater)
#define super BT_DECORATOR(self)
static BTRepeater* Init(BTRepeater* self, int remaining)
{
  bt_decorator_init((BTDecorator*)self);
  self->total     = remaining;
  self->remaining = remaining;
  return self;
}

void Abort(BTRepeater* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_decorator_abort((BTDecorator*)self, ecs, entity);
  self->remaining = self->total;
}

static BTStatus Execute(BTRepeater* self, Ecs* ecs, ecs_entity_t entity)
{
  BTStatus finish_status;
  if (self->remaining > 0)
  {
    self->remaining--;
    finish_status = bt_node_exec(super->child, ecs, entity);
    if (finish_status != BT_STATUS_RUNNING)
      bt_node_finish(super->child, ecs, entity, finish_status == BT_STATUS_SUCCESS);
    return (finish_status != BT_STATUS_FAILURE) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
  }
  return BT_STATUS_SUCCESS;
}

static void Finish(BTRepeater* self,
                   SDL_UNUSED Ecs*         ecs,
                   SDL_UNUSED ecs_entity_t entity,
                   SDL_UNUSED BOOL         succeed)
{
  self->remaining = self->total;
}

static void VtblInit(BTRepeaterVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->abort  = (BTAbortFunc)Abort;
  ((BTNodeVtbl*)vtbl)->exec   = (BTExecuteFunc)Execute;
  ((BTNodeVtbl*)vtbl)->finish = (BTFinishFunc)Finish;
}

BTDecorator* bt_repeater_new(int totals)
{
  return BT_DECORATOR(Init(Alloc(), totals));
}
