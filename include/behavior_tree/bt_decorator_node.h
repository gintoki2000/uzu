// ngotrung Sun 30 May 2021 08:31:32 AM +07
#ifndef BT_DECORATOR_NODE_H
#define BT_DECORATOR_NODE_H
#include "behavior_tree/bt_node.h"

typedef struct bt_DecoratorNode
{
  bt_Node  _base;
  bt_Node* child;
} bt_DecoratorNode;

typedef struct bt_DecoratorNodeRuntimeData
{
  BOOL isChildRunning;
} bt_DecoratorNodeRuntimeData;

#define BT_DECORATOR_NODE(ptr) ((bt_DecoratorNode*)ptr)

bt_DecoratorNode* bt_decorator_node_init(bt_DecoratorNode* self);
void              bt_decorator_node_fini(bt_DecoratorNode* self);
void              bt_decorator_node_on_finish(bt_DecoratorNode* self, const bt_UpdateContext* ctx);
bt_Status         bt_decorator_node_on_tick(bt_DecoratorNode* self, const bt_UpdateContext* ctx);
void              bt_decorator_node_attach(bt_DecoratorNode* self, bt_Node* node);
bt_Node*          bt_decorator_node_steal(bt_DecoratorNode* self);

#endif // BT_DECORATOR_NODE_H
