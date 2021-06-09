#include "behavior_tree/bt_decorator_node.h"

BT_NODE_DECL_EMPTY(bt_Inverter, bt_DecoratorNode)

#define super BT_DECORATOR_NODE(self)

static bt_Status on_tick(bt_Inverter* self, const bt_UpdateContext* ctx)
{
  switch (bt_decorator_node_on_tick(super, ctx))
  {
  case BT_STATUS_SUCCESS:
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  }
}

bt_DecoratorNode* bt_inverter_create(void)
{
  /*allocate memory*/
  bt_Inverter* self = SDL_malloc(sizeof(bt_Inverter));

  /*call super class constructor*/
  bt_decorator_node_init(super);

  /*overide virtual methods*/
  BT_NODE(self)->tick = (bt_OnTickFunc)on_tick;

  return (bt_DecoratorNode*)self;
}
