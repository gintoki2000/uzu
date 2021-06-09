#include "behavior_tree/bt_composite_node.h"
void bt_composite_node_accept(bt_CompositeNode* self, void* arg, bt_VisitFunc visitFunc);

bt_CompositeNode* bt_composite_node_init(bt_CompositeNode* self)
{
  /*chain construcion*/
  bt_node_init(BT_NODE(self), BT_NODE_TYPE_COMPOSITE);

  /*overide virtual methods*/
  BT_NODE(self)->fini   = (bt_FinalizeFunc)bt_composite_node_fini;
  BT_NODE(self)->accept = (bt_AcceptFunc)bt_composite_node_accept;

  /*init*/
  self->children = pointer_array_create((DestroyFunc)bt_node_free);
  return (self);
}

void bt_composite_node_fini(bt_CompositeNode* self)
{
  pointer_array_free(self->children);
  self->children = NULL;
  bt_node_fini(BT_NODE(self));
}

void bt_composite_node_add(bt_CompositeNode* self, bt_Node* node)
{
  ASSERT_MSG(node != NULL && node->parent == NULL, "invalid argument");
  pointer_array_add(self->children, node);
  node->parent = BT_NODE(self);
}

bt_Node* bt_composite_node_child_at(bt_CompositeNode* self, int index)
{
  return BT_NODE(self->children->storage[index]);
}

void bt_composite_node_accept(bt_CompositeNode* self, void* arg, bt_VisitFunc visitFunc)
{
  bt_Node** children;
  int       childCount;

  children   = (bt_Node**)self->children->storage;
  childCount = self->children->count;

  visitFunc(arg, (bt_Node*)self);
  for (int i = 0; i < childCount; ++i)
    bt_node_accept(children[i], arg, visitFunc);
}
