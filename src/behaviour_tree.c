#include "behaviour_tree.h"
#include <toolbox/toolbox.h>

BT_VTBL_INST_FN(bt_Node, bt_node)

void bt_node_vtbl_init(bt_NodeVtbl* vtbl)
{
  vtbl->parent = NULL;
  vtbl->abort  = bt_node_abort;
  vtbl->exec   = NULL;
  vtbl->fini   = (destroy_fn_t)bt_node_fini;
  vtbl->finish = bt_node_finish;
}

void bt_node_del(bt_Node* node)
{
  if (node)
  {
    node->vtbl->fini(node);
    free(node);
  }
}

bt_Node* bt_node_init(bt_Node* self)
{
  self->parent = NULL;
  return self;
}

void bt_node_fini(bt_Node* self)
{
  (void)self;
}

void bt_node_abort(bt_Node* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)self;
  (void)ecs;
  (void)entity;
}

void bt_node_finish(bt_Node* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)self;
  (void)ecs;
  (void)entity;
  (void)finish_status;
}

/*************************************************************/

BT_VTBL_INST_FN(bt_Node, bt_root)
BT_ALLOC_FN(bt_Root, bt_root)

void bt_root_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)bt_root_exec;
  vtbl->fini   = (destroy_fn_t)bt_root_fini;
}

bt_Root* bt_root_init(bt_Root* self)
{
  bt_node_init((bt_Node*)self);
  self->child = NULL;
  return self;
}

bt_Root* bt_root_new()
{
  return bt_root_init(bt_root_alloc());
}

void bt_root_fini(bt_Root* self)
{
  bt_node_del(self->child);
}

bt_Status bt_root_exec(bt_Root* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_Status finish_status = bt_node_vc_exec(self->child, ecs, entity);
  if (finish_status != BT_STATUS_RUNNING)
    bt_node_vc_finish(self->child, ecs, entity, finish_status);
  return BT_STATUS_RUNNING;
}

void bt_root_set_child(bt_Root* self, bt_Node* node)
{
  ASSERT(node->parent == NULL);
  node->parent = (bt_Node*)self;
  self->child  = node;
}

/*SELECTOR*/

BT_VTBL_INST_FN(bt_Node, bt_selector)
BT_ALLOC_FN(bt_Selector, bt_selector)

void bt_selector_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init((bt_NodeVtbl*)vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->fini   = (destroy_fn_t)bt_selector_fini;
  vtbl->abort  = (bt_abort_fn_t)bt_selector_abort;
  vtbl->exec   = (bt_exec_fn_t)bt_selector_exec;
  vtbl->finish = (bt_finish_fn_t)bt_selector_finish;
}

bt_Selector* bt_selector_new()
{
  return bt_selector_init(bt_selector_alloc());
}

bt_Selector* bt_selector_init(bt_Selector* self)
{
  bt_node_init(BT_NODE(self));
  self->children = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr     = 0;
  return self;
}

void bt_selector_fini(bt_Selector* self)
{
  ptr_array_delete(self->children);
  bt_node_fini((bt_Node*)self);
}

void bt_selector_abort(bt_Selector* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->children->cnt > 0)
    bt_node_vc_abort(self->children->storage[self->curr], ecs, entity);
  self->curr = 0;
}

static BOOL bt_selector_next_child(bt_Selector* self)
{
  if (self->curr < self->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}

bt_Status bt_selector_exec(bt_Selector* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_Status finish_status;
  bt_Node*  current_node;
  current_node  = ptr_array_at(self->children, self->curr);
  finish_status = bt_node_vc_exec(current_node, ecs, entity);
  switch (finish_status)
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_FAILURE:
    bt_node_vc_finish(current_node, ecs, entity, BT_STATUS_FAILURE);
    if (bt_selector_next_child(self))
    {
      return BT_STATUS_RUNNING;
    }
    else
    {
      return BT_STATUS_FAILURE;
    }
    break;
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(current_node, ecs, entity, BT_STATUS_SUCCESS);
    return BT_STATUS_SUCCESS;
  }
}

void bt_selector_finish(bt_Selector* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->curr = 0;
}

void bt_selector_add(bt_Selector* self, bt_Node* node)
{
  ASSERT(node->parent == NULL && "node already attach to other node");
  node->parent = (bt_Node*)self;
  ptr_array_add(self->children, node);
}

/*Sequence node*/

BT_VTBL_INST_FN(bt_Node, bt_sequence)
void bt_sequence_vtbl_init(bt_NodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->fini   = (destroy_fn_t)bt_sequence_fini;
  vtbl->abort  = (bt_abort_fn_t)bt_sequence_abort;
  vtbl->exec   = (bt_exec_fn_t)bt_sequence_exec;
  vtbl->finish = (bt_finish_fn_t)bt_sequence_finish;
}

bt_Sequence* bt_sequence_new()
{
  bt_Sequence* sequence   = malloc(sizeof(bt_Sequence));
  BT_NODE(sequence)->vtbl = bt_sequence_vtbl_inst();
  return bt_sequence_init(sequence);
}

bt_Sequence* bt_sequence_init(bt_Sequence* self)
{
  bt_node_init((bt_Node*)self);
  self->children = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr     = 0;
  return self;
}

void bt_sequence_fini(bt_Sequence* self)
{
  ptr_array_delete(self->children);
}

void bt_sequence_add(bt_Sequence* self, bt_Node* node)
{
  node->parent = (bt_Node*)self;
  ptr_array_add(self->children, node);
}

void bt_sequence_abort(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity)
{
  if (self->children->cnt > 0)
  {
    bt_node_vc_abort(self->children->storage[self->curr], ecs, entity);
  }
  self->curr = 0;
}

static BOOL bt_sequence_next_child(bt_Sequence* self)
{
  if (self->curr < self->children->cnt - 1)
  {
    self->curr++;
    return TRUE;
  }
  return FALSE;
}
bt_Status bt_sequence_exec(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_Node* current_node = ptr_array_at(self->children, self->curr);
  switch (bt_node_vc_exec(current_node, ecs, entity))
  {
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
    break;
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(current_node, ecs, entity, BT_STATUS_SUCCESS);
    if (bt_sequence_next_child(self))
    {
      return BT_STATUS_RUNNING;
    }
    else
    {
      return BT_STATUS_SUCCESS;
    }
    break;
  case BT_STATUS_FAILURE:
    bt_node_finish(current_node, ecs, entity, BT_STATUS_FAILURE);
    return BT_STATUS_FAILURE;
  }
}

void bt_sequence_finish(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->curr = 0;
}

/***********************************************************/

BT_VTBL_INST_FN(bt_Decorator, bt_decorator)

void bt_decorator_vtbl_init(bt_DecoratorVtbl* vtbl)
{
  bt_node_vtbl_init((bt_NodeVtbl*)vtbl);
  ((bt_NodeVtbl*)vtbl)->parent = bt_node_vtbl_inst();
  ((bt_NodeVtbl*)vtbl)->fini   = (destroy_fn_t)bt_decorator_fini;
  ((bt_NodeVtbl*)vtbl)->abort  = (bt_abort_fn_t)bt_decorator_abort;
}

bt_Decorator* bt_decorator_init(bt_Decorator* self)
{
  self                   = (bt_Decorator*)bt_node_init((bt_Node*)self);
  self->child            = NULL;
  self->is_child_running = FALSE;
  return self;
}

void bt_decorator_fini(bt_Decorator* self)
{
  if (self->child != NULL)
  {
    bt_node_del(self->child);
    self->child = NULL;
  }
}

void bt_decorator_abort(bt_Decorator* self, Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  if (self->child != NULL)
    bt_node_vc_abort(self->child, ecs, entity);
}

void bt_decorator_set_child(bt_Decorator* self, bt_Node* node)
{

  ASSERT(node->parent == NULL && "node already attach to other node");
  bt_decorator_del_child(self);
  self->child  = node;
  node->parent = (bt_Node*)self;
}

bt_Node* bt_decorator_steal_child(bt_Decorator* self)
{
  bt_Node* ret = self->child;
  if (self->child != NULL)
  {
    self->child->parent = NULL;
    self->child         = NULL;
  }
  return ret;
}

void bt_decorator_del_child(bt_Decorator* self)
{
  if (self->child != NULL)
  {
    bt_node_del(self->child);
    self->child = NULL;
  }
}

/***********************************************************/

BT_VTBL_INST_FN(bt_Repeater, bt_repeater)
BT_ALLOC_FN(bt_Repeater, bt_repeater)

static bt_Repeater* bt_repeater_init(bt_Repeater* self, int times)
{
  bt_decorator_init((bt_Decorator*)self);
  self->original_times = times;
  self->times          = times;
  return self;
}

void bt_repeater_vtbl_init(bt_RepeaterVtbl* vtbl)
{
  bt_decorator_vtbl_init((bt_DecoratorVtbl*)vtbl);
  ((bt_NodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((bt_NodeVtbl*)vtbl)->abort  = (bt_abort_fn_t)bt_repeater_abort;
  ((bt_NodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_repeater_exec;
  ((bt_NodeVtbl*)vtbl)->finish = (bt_finish_fn_t)bt_repeater_finish;
}

bt_Repeater* bt_repeater_new(int times)
{
  return bt_repeater_init(bt_repeater_alloc(), times);
}

void bt_repeater_abort(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_decorator_abort((bt_Decorator*)self, ecs, entity);
  self->times = self->original_times;
}

bt_Status bt_repeater_exec(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_Status finish_status;
  if (self->times > 0)
  {
    self->times--;
    finish_status = bt_node_vc_exec(((bt_Decorator*)self)->child, ecs, entity);
    if (finish_status != BT_STATUS_RUNNING)
      bt_node_vc_finish(((bt_Decorator*)self)->child, ecs, entity, finish_status);
    return BT_STATUS_RUNNING;
  }
  else
  {
    return BT_STATUS_SUCCESS;
  }
}

void bt_repeater_finish(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status)
{
  (void)ecs;
  (void)entity;
  (void)finish_status;
  self->times = self->original_times;
}

/**************************************************************/

BT_VTBL_INST_FN(bt_Inverter, bt_inverter)
BT_ALLOC_FN(bt_Inverter, bt_inverter)

void bt_inverter_vtbl_init(bt_InverterVtbl* vtbl)
{
  bt_decorator_vtbl_init((bt_DecoratorVtbl*)vtbl);
  ((bt_NodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((bt_NodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_inverter_exec;
}

static bt_Inverter* bt_inverter_init(bt_Inverter* self)
{
  return (bt_Inverter*)bt_decorator_init((bt_Decorator*)self);
}
bt_Inverter* bt_inverter_new()
{
  return bt_inverter_init(bt_inverter_alloc());
}

bt_Status bt_inverter_exec(bt_Inverter* self, Ecs* ecs, ecs_entity_t entity)
{
  switch (bt_node_vc_exec(((bt_Decorator*)self)->child, ecs, entity))
  {
  case BT_STATUS_SUCCESS:
    bt_node_vc_finish(((bt_Decorator*)self)->child, ecs, entity, BT_STATUS_SUCCESS);
    return BT_STATUS_FAILURE;
    break;
  case BT_STATUS_FAILURE:
    bt_node_vc_finish(((bt_Decorator*)self)->child, ecs, entity, BT_STATUS_SUCCESS);
    return BT_STATUS_SUCCESS;
    break;
  case BT_STATUS_RUNNING:
    return BT_STATUS_RUNNING;
  }
}

/************************************************************************/

BT_VTBL_INST_FN(bt_Condition, bt_condition)

void bt_condition_vtbl_init(bt_ConditionVtbl* vtbl)
{
  bt_decorator_vtbl_init((bt_DecoratorVtbl*)vtbl);
  ((bt_NodeVtbl*)vtbl)->parent = bt_decorator_vtbl_inst();
  ((bt_NodeVtbl*)vtbl)->exec   = (bt_exec_fn_t)bt_condition_exec;
  vtbl->pred                   = NULL;
}

bt_Condition* bt_condition_init(bt_Condition* self, BOOL abort_if_fail)
{
  bt_decorator_init((bt_Decorator*)self);
  self->abort_if_fail    = abort_if_fail;
  self->is_child_running = FALSE;
  return self;
}

bt_Status bt_condition_exec(bt_Condition* self, Ecs* ecs, ecs_entity_t entity)
{
  if (bt_condition_vc_pred(self, ecs, entity))
  {
    bt_Status stt = bt_node_vc_exec(((bt_Decorator*)self)->child, ecs, entity);
    if (stt == BT_STATUS_FAILURE || stt == BT_STATUS_SUCCESS)
      bt_node_vc_finish(self->__parent_inst.child, ecs, entity, stt);
    return stt;
  }
  else
  {
    if (self->abort_if_fail)
      bt_node_vc_abort(self, ecs, entity);
    return BT_STATUS_FAILURE;
  }
}
