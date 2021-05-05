#include "behaviour_tree.h"
BT_PUBLIC_NODE_IMPLEMENT(BTCompositeNode, bt_composite_node)

void bt_composite_node_vtbl_init(BTCompositeNodeVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->fini = (destroy_fn_t)bt_composite_node_init;
}

BTCompositeNode* bt_composite_node_init(BTCompositeNode* self)
{
  bt_node_init(BT_NODE(self));
  self->children = ptr_array_new((destroy_fn_t)bt_node_del);
  return (self);
}

void bt_composite_node_fini(BTCompositeNode* self)
{
  ptr_array_delete(self->children);
  self->children = NULL;
  bt_node_fini(BT_NODE(self));
}

void bt_composite_node_add(BTCompositeNode* self, BTNode* node)
{
  ASSERT_MSG(node != NULL && node->parent == NULL, "invalid argument");
  ptr_array_add(self->children, node);
  node->parent = BT_NODE(self);
}

BTNode* bt_composite_node_child_at(BTCompositeNode* self, int index)
{
  return BT_NODE(self->children->storage[index]);
}
