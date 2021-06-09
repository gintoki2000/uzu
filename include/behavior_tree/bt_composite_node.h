// ngotrung Sun 30 May 2021 08:15:09 AM +07
#ifndef BT_COMPOSITE_NODE_H
#define BT_COMPOSITE_NODE_H
#include "behavior_tree/bt_node.h"

#define BT_COMPOSITE_NODE(ptr) ((bt_CompositeNode*)ptr)

BT_NODE_DECL(bt_CompositeNode, bt_Node, { PointerArray* children; })

bt_CompositeNode* bt_composite_node_init(bt_CompositeNode* self);
void              bt_composite_node_fini(bt_CompositeNode* self);
void              bt_composite_node_add(bt_CompositeNode* self, bt_Node* node);
bt_Node*          bt_composite_node_child_at(bt_CompositeNode* self, int index);
#endif // BT_COMPOSITE_NODE_H
