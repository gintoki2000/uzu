#include "behaviour_tree.h"
#include <toolbox/toolbox.h>

#define set_running bt_node_set_running
#define set_fail bt_node_set_fail
#define set_succesed bt_node_set_succesed

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
  self->status = BT_STATUS_NOT_EXECUTED;
  return self;
}

void bt_node_start_impl(bt_Node* self) { self->status = BT_STATUS_RUNNING; }

void bt_node_fini(bt_Node* self) { (void)self; }

/*Slector node*/

static const bt_NodeVtbl BT_SELECTOR_VTBL = {
  .fini  = (destroy_fn_t)bt_selector_fini,
  .exec  = (bt_exec_fn_t)bt_selector_exec,
  .start = (bt_start_fn_t)bt_selector_start,
};

bt_Selector* bt_selector_new()
{
  bt_Selector* selector   = malloc(sizeof(bt_Selector));
  BT_NODE(selector)->vtbl = &BT_SELECTOR_VTBL;
  return bt_selector_init(selector);
}

bt_Selector* bt_selector_init(bt_Selector* self)
{
  bt_node_init(BT_NODE(self));
  self->children  = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr_idx  = -1;
  self->curr_node = NULL;
  return self;
}

void bt_selector_fini(bt_Selector* self)
{
  ptr_array_delete(self->children);
  bt_node_fini((bt_Node*)self);
}

INLINE bt_Node* bt_selector_next(bt_Selector* self)
{
  ASSERT(self->curr_idx >= 0 && "you must start this call start first");
  if (self->curr_idx < self->children->cnt - 1)
  {
    ++self->curr_idx;
    return ptr_array_at(self->children, self->curr_idx);
  }
  return NULL;
}

void bt_selector_start(bt_Selector* self)
{
  bt_node_start_impl((bt_Node*)self);
  self->curr_idx  = 0;
  self->curr_node = ptr_array_at(self->children, 0);
  bt_node_start(self->curr_node);
}

void bt_selector_exec(bt_Selector* self, Ecs* ecs, ecs_entity_t entity)
{
  bt_node_exec(self->curr_node, ecs, entity);
  bt_Status stt = bt_node_stt(self->curr_node);
  if (stt == BT_STATUS_SUCCESS)
  {
    bt_node_set_succesed(self);
    return;
  }
  if (stt == BT_STATUS_RUNNING)
  {
    return;
  }
  if ((self->curr_node = bt_selector_next(self)) != NULL)
  {
    bt_node_start(self->curr_node);
  }
  else
  {
    bt_node_set_fail(self);
  }
}

/*Sequence node*/

static bt_NodeVtbl BT_SEQUENCE_VTBL = {
  .fini  = (destroy_fn_t)bt_sequence_fini,
  .start = (bt_start_fn_t)bt_sequence_start,
  .exec  = (bt_exec_fn_t)bt_sequence_exec,
};

bt_Sequence* bt_sequence_new()
{
  bt_Sequence* sequence   = malloc(sizeof(bt_Sequence));
  BT_NODE(sequence)->vtbl = &BT_SEQUENCE_VTBL;
  return bt_sequence_init(sequence);
}

bt_Sequence* bt_sequence_init(bt_Sequence* self)
{
  bt_node_init((bt_Node*)self);
  self->children  = ptr_array_new((destroy_fn_t)bt_node_del);
  self->curr_idx  = -1;
  self->curr_node = NULL;
  return self;
}

void bt_sequence_fini(bt_Sequence* self) { ptr_array_delete(self->children); }

void bt_sequence_add(bt_Sequence* self, bt_Node* node) { ptr_array_add(self->children, node); }

INLINE bt_Node* bt_sequence_next(bt_Sequence* self)
{
  ASSERT(self->curr_idx >= 0 && "you must call start first");
  if (self->curr_idx < self->children->cnt - 1)
  {
    ++self->curr_idx;
    return ptr_array_at(self->children, self->curr_idx);
  }
  return NULL;
}

void bt_sequence_start(bt_Sequence* self)
{
  bt_node_start_impl((bt_Node*)self);
  ASSERT(self->children->cnt > 0 && "sequence must have at least one child");
  self->curr_idx  = 0;
  self->curr_node = ptr_array_at(self->children, 0);
  bt_node_start(self->curr_node);
}

void bt_sequence_exec(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity)
{
  //INFO("executing...\n");
  ASSERT(self->curr_node != NULL);
  bt_node_exec(self->curr_node, ecs, entity);
  bt_Status stt = bt_node_stt(self->curr_node);

  if (stt == BT_STATUS_FAILURE || stt == BT_STATUS_RUNNING)
  {
    bt_node_set_stt(self, stt);
    return;
  }

  if ((self->curr_node = bt_sequence_next(self)) != NULL)
  {
    bt_node_start(self->curr_node);
  }
  else
  {
    bt_node_set_succesed(self);
  }
}

/*repeater node*/
static const bt_NodeVtbl BT_REPEATER_VTBL = {
  .fini  = (destroy_fn_t)bt_repeater_fini,
  .start = (bt_start_fn_t)bt_repeater_start,
  .exec  = (bt_exec_fn_t)bt_repeater_exec,
};

static bt_Repeater* bt_repeater_init(bt_Repeater* self, bt_Node* node, int times);

bt_Repeater* bt_repeater_new(bt_Node* node, int times)
{
  bt_Repeater* repeater   = malloc(sizeof(bt_Repeater));
  BT_NODE(repeater)->vtbl = &BT_REPEATER_VTBL;
  return bt_repeater_init(repeater, node, times);
}

static bt_Repeater* bt_repeater_init(bt_Repeater* self, bt_Node* node, int times)
{
  bt_node_init((bt_Node*)self);
  self->original_times = times;
  self->times          = times;
  self->node           = node;
  return self;
}

void bt_repeater_fini(bt_Repeater* self)
{
  bt_node_del(self->node);
  bt_node_fini((bt_Node*)self);
}

void bt_repeater_start(bt_Repeater* self)
{
  bt_node_start_impl((bt_Node*)self);
  self->times = self->original_times;
  bt_node_start(self->node);
}

void bt_repeater_exec(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity)
{
  if (bt_node_is_running(self->node))
    bt_node_exec(self->node, ecs, entity);

  bt_Status stt = bt_node_stt(self->node);
  if (stt == BT_STATUS_FAILURE || stt == BT_STATUS_SUCCESS)
  {
    if (self->times == 0)
    {
      set_succesed(self);
      return;
    }
    else if (self->times > 0)
    {
      --self->times;
      bt_node_start(self->node);
    }
    else if (self->times == -1)
    {
      bt_node_start(self->node);
    }
  }
}

static const bt_NodeVtbl BT_INVERTER_VTBL = {
  .fini  = (destroy_fn_t)bt_inverter_fini,
  .start = (bt_start_fn_t)bt_inverter_start,
  .exec  = (bt_exec_fn_t)bt_inverter_exec,
};

static bt_Inverter* bt_inverter_init(bt_Inverter* self, bt_Node* node);

bt_Inverter* bt_inverter_new(bt_Node* node)
{
  bt_Inverter* inverter   = malloc(sizeof(bt_Inverter));
  BT_NODE(inverter)->vtbl = &BT_INVERTER_VTBL;
  return bt_inverter_init(inverter, node);
}

static bt_Inverter* bt_inverter_init(bt_Inverter* self, bt_Node* node)
{
  bt_node_init((bt_Node*)self);
  self->node = node;
  return self;
}

void bt_inverter_fini(bt_Inverter* self)
{
  bt_node_del(self->node);
  self->node = NULL;
}
void bt_inverter_start(bt_Inverter* self)
{
  bt_node_start_impl((bt_Node*)self);
  bt_node_start(self->node);
}
void bt_inverter_exec(bt_Inverter* self, Ecs* ecs, ecs_entity_t entity)
{
  if (bt_node_is_running(self->node))
    bt_node_exec(self->node, ecs, entity);
  bt_Status stt = bt_node_stt(self->node);
  if (stt == BT_STATUS_FAILURE)
  {
    bt_node_set_stt(self, BT_STATUS_SUCCESS);
    return;
  }

  if (stt == BT_STATUS_SUCCESS)
  {
    bt_node_set_stt(self, BT_STATUS_FAILURE);
    return;
  }
}

#undef set_fail
#undef set_running
#undef set_succesed
