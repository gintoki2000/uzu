#include "behavior_tree/base.h"
BT_GLOBAL_VTBL_INITIALIZER_IMPL(BTCompositeNode, bt_composite_node, BTNode, bt_node, {
  ((BTNodeVtbl*)vtbl)->fini = (BTFinalizeFunc)bt_composite_node_fini;
})

BTCompositeNode* bt_composite_node_init(BTCompositeNode* self)
{
  bt_node_init(BT_NODE(self));
  self->children = ptr_array_new((DestroyFunc)bt_node_del);
  return (self);
}

void bt_composite_node_fini(BTCompositeNode* self)
{
  ptr_array_delete(self->children);
  self->children = NULL;
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
