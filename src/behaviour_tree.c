#include "behaviour_tree.h"
#include <toolbox/toolbox.h>

void bt_node_vtbl_init(BTNodeVtbl* vtbl)
{
  vtbl->abort  = bt_node_real_abort;
  vtbl->exec   = NULL;
  vtbl->fini   = (BTFinalizeFunc)bt_node_real_fini;
  vtbl->finish = bt_node_real_finish;
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

void bt_node_real_fini(SDL_UNUSED BTNode* self)
{
}

void bt_node_real_abort(SDL_UNUSED BTNode* self,
                        SDL_UNUSED Ecs*         ecs,
                        SDL_UNUSED ecs_entity_t entity)
{
}

void bt_node_real_finish(SDL_UNUSED BTNode* self,
                         SDL_UNUSED Ecs*         ecs,
                         SDL_UNUSED ecs_entity_t entity,
                         SDL_UNUSED BOOL         succeed)
{
}


/************************************************************************/

void bt_condition_vtbl_init(BTConditionVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((BTNodeVtbl*)vtbl)->exec   = (BTExecuteFunc)bt_condition_exec;
  vtbl->pred                  = NULL;
}

BTCondition* bt_condition_init(BTCondition* self, BOOL abort_if_fail, BOOL invert_result)
{
  bt_decorator_init((BTDecorator*)self);
  self->abort_if_fail    = abort_if_fail;
  self->is_child_running = FALSE;
  self->invert_result    = invert_result;
  return self;
}

BTStatus bt_condition_exec(BTCondition* self, Ecs* ecs, ecs_entity_t entity)
{
  BOOL predict_result = bt_condition_vc_pred(self, ecs, entity);
  predict_result      = self->invert_result ? !predict_result : predict_result;
  if (predict_result)
  {
    BTStatus stt = bt_node_exec(super->child, ecs, entity);
    if (stt == BT_STATUS_FAILURE || stt == BT_STATUS_SUCCESS)
      bt_node_finish(super->child, ecs, entity, stt == BT_STATUS_SUCCESS);
    return stt;
  }
  else
  {
    if (self->abort_if_fail)
      bt_node_abort(self, ecs, entity);
    return BT_STATUS_FAILURE;
  }
}
