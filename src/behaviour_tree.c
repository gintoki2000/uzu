#include "behaviour_tree.h"
#include <toolbox/toolbox.h>

BT_VTBL_INST_FN(BTNode, bt_node)

void bt_node_vtbl_init(BTNodeVtbl* vtbl)
{
  vtbl->parent = NULL;
  vtbl->abort  = bt_node_abort;
  vtbl->exec   = NULL;
  vtbl->fini   = (destroy_fn_t)bt_node_fini;
  vtbl->finish = bt_node_finish;
}

void bt_node_del(BTNode* node)
{
  if (node)
  {
    node->vtbl->fini(node);
    free(node);
  }
}

BTNode* bt_node_init(BTNode* self)
{
  self->parent = NULL;
  return self;
}

void bt_node_fini(BTNode* self)
{
  (void)self;
}

void bt_node_abort(BTNode* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;
  (void)ecs;
  (void)entity;
}

void bt_node_finish(SDL_UNUSED BTNode* self,
                    SDL_UNUSED Ecs*         ecs,
                    SDL_UNUSED ecs_entity_t entity,
                    SDL_UNUSED BOOL         succeed)
{
}

/*************************************************************/

BT_VTBL_INST_FN(BTNode, bt_root)
BT_ALLOC_FN(BTRoot, bt_root)

void bt_root_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)bt_root_exec;
  vtbl->fini   = (destroy_fn_t)bt_root_fini;
}

BTRoot* bt_root_init(BTRoot* self)
{
  bt_node_init((BTNode*)self);
  self->child = NULL;
  return self;
}

BTRoot* bt_root_new()
{
  return bt_root_init(bt_root_alloc());
}

void bt_root_fini(BTRoot* self)
{
  bt_node_del(self->child);
}

BTStatus bt_root_exec(BTRoot* self, Ecs* ecs, ecs_entity_t entity)
{
  BTStatus finish_status = bt_node_vc_exec(self->child, ecs, entity);
  if (finish_status != BT_STATUS_RUNNING)
    bt_node_vc_finish(self->child, ecs, entity, finish_status == BT_STATUS_SUCCESS);
  return BT_STATUS_RUNNING;
}

void bt_root_set_child(BTRoot* self, BTNode* node)
{
  ASSERT(node->parent == NULL);
  node->parent = (BTNode*)self;
  self->child  = node;
}

/*SELECTOR*/

BT_VTBL_INST_FN(BTNode, bt_selector)
BT_ALLOC_FN(BTSelector, bt_selector)

void bt_selector_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->fini   = (destroy_fn_t)bt_selector_fini;
  vtbl->abort  = (bt_abort_fn_t)bt_selector_abort;
  vtbl->exec   = (bt_exec_fn_t)bt_selector_exec;
  vtbl->finish = (bt_finish_fn_t)bt_selector_finish;
}

BTSelector* bt_selector_new()
{
  return bt_selector_init(bt_selector_alloc());
}

BTSelector* bt_selector_init(BTSelector* self)
{
  bt_node_init(BT_NODE(self));
  self->children = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr     = 0;
  return self;
}

void bt_selector_fini(BTSelector* self)
{
  ptr_array_delete(self->children);
  bt_node_fini((BTNode*)self);
}

void bt_selector_abort(BTSelector* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->children->cnt > 0)
    bt_node_vc_abort(self->children->storage[self->curr], ecs, entity);
  self->curr = 0;
}

static BOOL bt_selector_next_child(BTSelector* self)
{
  if (self->curr < self->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}

BTStatus bt_selector_exec(BTSelector* self, Ecs* ecs, ecs_entity_t entity)
{
  BTStatus finish_status;
  BTNode*  current_node;
  current_node  = ptr_array_at(self->children, self->curr);
  finish_status = bt_node_vc_exec(current_node, ecs, entity);
  switch (finish_status)
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_FAILURE:
    bt_node_vc_finish(current_node, ecs, entity, FALSE);
    return bt_selector_next_child(self) ? BT_STATUS_RUNNING : BT_STATUS_FAILURE;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(current_node, ecs, entity, TRUE);
    return BT_STATUS_SUCCESS;
  }
}

void bt_selector_finish(BTSelector* self,
                        SDL_UNUSED Ecs*         ecs,
                        SDL_UNUSED ecs_entity_t entity,
                        SDL_UNUSED BOOL         succeed)
{
  self->curr = 0;
}

void bt_selector_add(BTSelector* self, BTNode* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  node->parent = (BTNode*)self;
  ptr_array_add(self->children, node);
}

/*Sequence node*/

BT_VTBL_INST_FN(BTNode, bt_sequence)
void bt_sequence_vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->fini   = (destroy_fn_t)bt_sequence_fini;
  vtbl->abort  = (bt_abort_fn_t)bt_sequence_abort;
  vtbl->exec   = (bt_exec_fn_t)bt_sequence_exec;
  vtbl->finish = (bt_finish_fn_t)bt_sequence_finish;
}

BTSequence* bt_sequence_new()
{
  BTSequence* sequence    = malloc(sizeof(BTSequence));
  BT_NODE(sequence)->vtbl = bt_sequence_vtbl_inst();
  return bt_sequence_init(sequence);
}

BTSequence* bt_sequence_init(BTSequence* self)
{
  bt_node_init((BTNode*)self);
  self->children = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr     = 0;
  return self;
}

void bt_sequence_fini(BTSequence* self)
{
  ptr_array_delete(self->children);
}

void bt_sequence_add(BTSequence* self, BTNode* node)
{
  node->parent = (BTNode*)self;
  ptr_array_add(self->children, node);
}

void bt_sequence_abort(BTSequence* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->children->cnt > 0)
  {
    bt_node_vc_abort(self->children->storage[self->curr], ecs, entity);
  }
  self->curr = 0;
}

static BOOL bt_sequence_next_child(BTSequence* self)
{
  if (self->curr < self->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}
BTStatus bt_sequence_exec(BTSequence* self, Ecs* ecs, ecs_entity_t entity)
{
  BTNode* current_node = ptr_array_at(self->children, self->curr);
  switch (bt_node_vc_exec(current_node, ecs, entity))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(current_node, ecs, entity, TRUE);
    return bt_sequence_next_child(self) ? BT_STATUS_RUNNING : BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(current_node, ecs, entity, FALSE);
    return BT_STATUS_FAILURE;
  }
}

void bt_sequence_finish(BTSequence* self,
                        SDL_UNUSED Ecs*         ecs,
                        SDL_UNUSED ecs_entity_t entity,
                        SDL_UNUSED BOOL         succeed)
{
  self->curr = 0;
}

/***********************************************************/

BT_VTBL_INST_FN(BTDecorator, bt_decorator)

void bt_decorator_vtbl_init(BTDecoratorVtbl* vtbl)
{
  bt_node_vtbl_init((BTNodeVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_node_vtbl_inst();
  ((BTNodeVtbl*)vtbl)->fini   = (destroy_fn_t)bt_decorator_fini;
  ((BTNodeVtbl*)vtbl)->abort  = (bt_abort_fn_t)bt_decorator_abort;
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

void bt_decorator_abort(BTDecorator* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  if (self->child != NULL)
    bt_node_vc_abort(self->child, ecs, entity);
}

void bt_decorator_set_child(BTDecorator* self, BTNode* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  bt_decorator_del_child(self);
  self->child  = node;
  node->parent = (BTNode*)self;
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

void bt_decorator_del_child(BTDecorator* self)
{
  if (self->child != NULL)
  {
    bt_node_del(self->child);
    self->child = NULL;
  }
}

/***********************************************************/

BT_VTBL_INST_FN(BTRepeater, bt_repeater)
BT_ALLOC_FN(BTRepeater, bt_repeater)

#define SUPER ((BTDecorator*)self)
static BTRepeater* bt_repeater_init(BTRepeater* self, int times)
{
  bt_decorator_init((BTDecorator*)self);
  self->original_times = times;
  self->times          = times;
  return self;
}

void bt_repeater_vtbl_init(BTRepeaterVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((BTNodeVtbl*)vtbl)->abort  = (bt_abort_fn_t)bt_repeater_abort;
  ((BTNodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_repeater_exec;
  ((BTNodeVtbl*)vtbl)->finish = (bt_finish_fn_t)bt_repeater_finish;
}

BTRepeater* bt_repeater_new(int times)
{
  return bt_repeater_init(bt_repeater_alloc(), times);
}

void bt_repeater_abort(BTRepeater* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_decorator_abort((BTDecorator*)self, ecs, entity);
  self->times = self->original_times;
}

BTStatus bt_repeater_exec(BTRepeater* self, Ecs* ecs, ecs_entity_t entity)
{
  BTStatus finish_status;
  if (self->times > 0)
  {
    self->times--;
    finish_status = bt_node_vc_exec(SUPER->child, ecs, entity);
    if (finish_status != BT_STATUS_RUNNING)
      bt_node_vc_finish(SUPER->child, ecs, entity, finish_status == BT_STATUS_SUCCESS);
    return BT_STATUS_RUNNING;
  }
  else
  {
    return BT_STATUS_SUCCESS;
  }
}

void bt_repeater_finish(BTRepeater* self,
                        SDL_UNUSED Ecs*         ecs,
                        SDL_UNUSED ecs_entity_t entity,
                        SDL_UNUSED BOOL         succeed)
{
  self->times = self->original_times;
}

/**************************************************************/
BT_VTBL_INST_FN(BTInverter, bt_inverter)
BT_ALLOC_FN(BTInverter, bt_inverter)

void bt_inverter_vtbl_init(BTInverterVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((BTNodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_inverter_exec;
}

static BTInverter* bt_inverter_init(BTInverter* self)
{
  return (BTInverter*)bt_decorator_init((BTDecorator*)self);
}
BTInverter* bt_inverter_new()
{
  return bt_inverter_init(bt_inverter_alloc());
}

BTStatus bt_inverter_exec(BTInverter* self, Ecs* ecs, ecs_entity_t entity)
{
  switch (bt_node_vc_exec(SUPER->child, ecs, entity))
  {
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(SUPER->child, ecs, entity, TRUE);
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    bt_node_vc_finish(SUPER->child, ecs, entity, FALSE);
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  }
}

/************************************************************************/

BT_VTBL_INST_FN(BTCondition, bt_condition)

void bt_condition_vtbl_init(BTConditionVtbl* vtbl)
{
  bt_decorator_vtbl_init((BTDecoratorVtbl*)vtbl);
  ((BTNodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((BTNodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_condition_exec;
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
    BTStatus stt = bt_node_vc_exec(SUPER->child, ecs, entity);
    if (stt == BT_STATUS_FAILURE || stt == BT_STATUS_SUCCESS)
      bt_node_vc_finish(SUPER->child, ecs, entity, stt == BT_STATUS_SUCCESS);
    return stt;
  }
  else
  {
    if (self->abort_if_fail)
      bt_node_vc_abort(self, ecs, entity);
    return BT_STATUS_FAILURE;
  }
}
