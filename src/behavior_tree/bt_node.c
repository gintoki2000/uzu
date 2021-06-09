#include "behavior_tree/bt_node.h"

void bt_node_free(bt_Node* node)
{
  if (node)
  {
    node->fini(node);
    SDL_free(node);
  }
}

bt_Node* bt_node_init(bt_Node* self, bt_NodeType type)
{
  self->type   = type;
  self->fini   = (bt_FinalizeFunc)bt_node_fini;
  self->finish = (bt_OnFinishFunc)bt_node_on_finish;
  self->tick   = (bt_OnTickFunc)bt_node_on_tick;
  self->start  = (bt_OnStartFunc)bt_node_on_start;
  self->accept = (bt_AcceptFunc)bt_node_real_accept;
  self->parent = NULL;
  self->tree   = NULL;
  self->id     = 0;
  return self;
}

void bt_node_fini(SDL_UNUSED bt_Node* self)
{
}

void bt_node_on_finish(SDL_UNUSED void* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
}

void bt_node_on_start(SDL_UNUSED void* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
}

bt_Status bt_node_on_tick(SDL_UNUSED void* self, SDL_UNUSED const bt_UpdateContext* ctx)
{
  return BT_STATUS_SUCCESS;
}

void bt_node_real_accept(void* self, void* arg, bt_VisitFunc visitFunc)
{
  visitFunc(arg, (bt_Node*)self);
}
