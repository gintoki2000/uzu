#include "behaviour_tree.h"

typedef struct
{
  BTDecorator _base;
} BTInverter;

typedef struct
{
  BTDecoratorVtbl _base;
} BTInverterVtbl;

#define super BT_DECORATOR(self)

static BTInverter* Init(BTInverter* self);
static BTStatus    Execute(BTInverter* self, Ecs* registry, ecs_entity_t entity);

BT_PRIVATE_NODE(BTInverter)

static void VtblInit(BTInverterVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->exec = (BTExecuteFunc)Execute;
}

BTDecorator* bt_inverter_new()
{
  return BT_DECORATOR(Init(Alloc()));
}

static BTInverter* Init(BTInverter* self)
{
  bt_decorator_init(super);
  return self;
}

static BTStatus Execute(BTInverter* self, Ecs* ecs, ecs_entity_t entity)
{
  switch (bt_node_exec(super->child, ecs, entity))
  {
  case BT_STATUS_SUCCESS:
    bt_node_finish(super->child, ecs, entity, TRUE);
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(super->child, ecs, entity, FALSE);
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  }
}
