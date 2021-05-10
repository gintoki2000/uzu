#include "behavior_tree.h"
#include <toolbox/toolbox.h>

void bt_node_vtbl_init(BTNodeVtbl* vtbl)
{
  vtbl->fini   = (BTFinalizeFunc)bt_node_fini;
  vtbl->finish = (BTOnFinishFunc)bt_node_on_finish;
  vtbl->tick   = (BTOnTickFunc)bt_node_on_tick;
  vtbl->start  = (BTOnStartFunc)bt_node_on_start;
}

void bt_node_del(BTNode* node)
{
  if (node)
  {
    node->vtbl->fini(node);
    SDL_free(node);
  }
}

BTNode* bt_node_init(BTNode* self)
{
  self->parent = NULL;
  return self;
}

void bt_node_fini(SDL_UNUSED BTNode* self)
{
}

void bt_node_on_finish(SDL_UNUSED BTNode* self,
                       SDL_UNUSED const BTUpdateContext* ctx,
                       SDL_UNUSED BOOL                   succeed)
{
}

void bt_node_on_start(SDL_UNUSED BTNode* self, SDL_UNUSED const BTUpdateContext* ctx)
{
}

BTStatus bt_node_on_tick(SDL_UNUSED BTNode* self, SDL_UNUSED const BTUpdateContext* ctx)
{
  return BT_STATUS_SUCCESS;
}
