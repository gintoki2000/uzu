#ifndef BEHAVIOUR_TREE_H
#define BEHAVIOUR_TREE_H

#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

typedef enum
{
  BT_STATUS_NOT_EXECUTED,
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} bt_Status;

typedef struct bt_Node bt_Node;

typedef void (*bt_exec_fn_t)(bt_Node*, Ecs*, ecs_entity_t);
typedef void (*bt_start_fn_t)(bt_Node*);

typedef struct bt_NodeVtbl
{
  destroy_fn_t  fini;
  bt_exec_fn_t  exec;
  bt_start_fn_t start;
} bt_NodeVtbl;

typedef struct bt_Node
{
  const bt_NodeVtbl* vtbl;
  bt_Status          status;
} bt_Node;

#define __EXTEND_BT_NODE__ bt_Node __parent_inst;
#define BT_NODE(__node) ((bt_Node*)__node)
#define bt_node_exec(__node, __ecs, __entity)                                                      \
  (BT_NODE(__node)->vtbl->exec(BT_NODE(__node), __ecs, __entity))
#define bt_node_start(__node) (BT_NODE(__node)->vtbl->start(BT_NODE(__node)))
#define bt_node_set_stt(__node, __stt) (BT_NODE(__node)->status = (__stt))
#define bt_node_stt(__node) (BT_NODE(__node)->status)
#define bt_node_is_running(__node) (bt_node_stt(__node) == BT_STATUS_RUNNING)
#define bt_node_is_fail(__node) (bt_node_stt(__node) == BT_STATUS_FAILURE)
#define bt_node_is_succesed(__node) (bt_node_stt(__node) == BT_STATUS_SUCCESS)
#define bt_node_set_succesed(__self) (bt_node_set_stt(__self, BT_STATUS_SUCCESS))
#define bt_node_set_fail(__self) (bt_node_set_stt(__self, BT_STATUS_FAILURE))
#define bt_node_set_running(__self) (bt_node_set_stt(__self, BT_STATUS_RUNNING))

void     bt_node_del(bt_Node* node);
bt_Node* bt_node_init(bt_Node* self);
void     bt_node_fini(bt_Node* self);
void     bt_node_start_impl(bt_Node* node);

/*==COMPOSITE==*/

typedef struct bt_Selector
{
  __EXTEND_BT_NODE__
  PtrArray* children;
  s32       curr_idx;
  bt_Node*  curr_node;
} bt_Selector;

bt_Selector* bt_selector_new();
bt_Selector* bt_selector_init(bt_Selector* self);
void         bt_selector_fini(bt_Selector* self);
void         bt_selector_start(bt_Selector* self);
void         bt_selector_exec(bt_Selector* self, Ecs* ecs, ecs_entity_t entity);
void         bt_selector_add(bt_Selector* self, bt_Node* node);

typedef struct bt_Sequence
{
  bt_Node   __parent_inst;
  PtrArray* children;
  s32       curr_idx;
  bt_Node*  curr_node;
} bt_Sequence;

bt_Sequence* bt_sequence_new();
bt_Sequence* bt_sequence_init(bt_Sequence* self);
void         bt_sequence_fini(bt_Sequence* self);
void         bt_sequence_start(bt_Sequence* self);
void         bt_sequence_exec(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity);
void         bt_sequence_add(bt_Sequence* self, bt_Node* node);

/*==DECORATOR==*/

typedef struct bt_Repeater
{
  bt_Node  __parent_inst;
  bt_Node* node;
  int      original_times;
  int      times;
} bt_Repeater;

bt_Repeater* bt_repeater_new(bt_Node* node, int times);
void         bt_repeater_fini(bt_Repeater* self);
void         bt_repeater_start(bt_Repeater* self);
void         bt_repeater_exec(bt_Repeater* self, Ecs* ecs, ecs_entity_t);

typedef struct bt_Inverter
{
  bt_Node  __parent_inst;
  bt_Node* node;
} bt_Inverter;

bt_Inverter* bt_inverter_new(bt_Node* node);
void         bt_inverter_fini(bt_Inverter* self);
void         bt_inverter_start(bt_Inverter* self);
void         bt_inverter_exec(bt_Inverter* self, Ecs* ecs, ecs_entity_t entity);

#endif // BEHAVIOUR_TREE_H
