#include "behaviour_tree.h"
BT_PUBLIC_NODE_IMPLEMENT(BTDecorator, bt_decorator)
void bt_decorator_vtbl_init(BTDecoratorVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->fini  = (BTFinalizeFunc)bt_decorator_fini;
  ((BTNodeVtbl*)vtbl)->abort = (BTAbortFunc)bt_decorator_abort;
}

BTDecorator* bt_decorator_init(BTDecorator* self)
{
  self                   = (BTDecorator*)bt_node_init((BTNode*)self);
  self->child            = NULL;
  self->is_child_running = FALSE;
  return self;
}

void bt_decorator_fini(BTDecorator* self)
{
  if (self->child != NULL)
  {
    bt_node_del(self->child);
    self->child = NULL;
  }
}

void bt_decorator_abort(BTDecorator* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
  if (self->child != NULL)
    bt_node_abort(self->child, ecs, entity);
}

void bt_decorator_set_child(BTDecorator* self, BTNode* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  if (self->child != NULL)
    bt_node_del(self->child);
  self->child  = node;
  node->parent = BT_NODE(self);
}

BTNode* bt_decorator_steal_child(BTDecorator* self)
{
  BTNode* ret = self->child;
  if (self->child != NULL)
  {
    self->child->parent = NULL;
    self->child         = NULL;
  }
  return ret;
}
