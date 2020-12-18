#ifndef BEHAVIOUR_TREE_H
#define BEHAVIOUR_TREE_H

#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} bt_Status;

typedef struct bt_Node bt_Node;

typedef bt_Status (*bt_exec_fn_t)(bt_Node*, Ecs*, ecs_entity_t);
typedef void (*bt_abort_fn_t)(bt_Node*, Ecs* ecs, ecs_entity_t);
typedef void (*bt_finish_fn_t)(bt_Node*, Ecs* ecs, ecs_entity_t, bt_Status finish_status);

typedef struct bt_NodeVtbl
{
  const struct bt_NodeVtbl* parent;
  destroy_fn_t              fini;
  bt_exec_fn_t              exec;
  bt_abort_fn_t             abort;
  bt_finish_fn_t            finish;
} bt_NodeVtbl;

#define BT_VTBL_INST_FN(__TName, __tname)                                                          \
  const bt_NodeVtbl* __tname##_vtbl_inst()                                                         \
  {                                                                                                \
    static __TName##Vtbl vtbl;                                                                     \
    static BOOL          initialized = FALSE;                                                      \
    if (!initialized)                                                                              \
    {                                                                                              \
      __tname##_vtbl_init(&vtbl);                                                                  \
      initialized = TRUE;                                                                          \
    }                                                                                              \
    return (bt_NodeVtbl*)&vtbl;                                                                    \
  }

const bt_NodeVtbl* bt_node_vtbl_inst();
void               bt_node_vtbl_init(bt_NodeVtbl* vtbl);

typedef struct bt_Node
{
  const bt_NodeVtbl* vtbl;
  bt_Node*           parent;
} bt_Node;

#define BT_EXTEND_NODE(__base) __base __parent_inst;
#define BT_EXTEND_VTBL(__bvtbl) __bvtbl __parent_vtbl;
#define BT_NODE(__node) ((bt_Node*)__node)
#define BT_GET_VTBL(__node) (BT_NODE(__node)->vtbl)

/**
 * execute this node
 */
#define bt_node_vc_exec(__node, __ecs, __entity)                                                   \
  (BT_NODE(__node)->vtbl->exec(BT_NODE(__node), __ecs, __entity))

/**
 * abort this node
 */
#define bt_node_vc_abort(__node, __ecs, __entity)                                                  \
  (BT_NODE(__node)->vtbl->abort(BT_NODE(__node), __ecs, __entity))

/**
 * called when this node is finished
 */
#define bt_node_vc_finish(__node, __ecs, __entity, __finish_status)                                \
  (BT_NODE(__node)->vtbl->finish(BT_NODE(__node), __ecs, __entity, __finish_status))

#define BT_ALLOC_FN(__TName, __tname)                                                              \
  __TName* __tname##_alloc()                                                                       \
  {                                                                                                \
    __TName* ret       = malloc(sizeof(__TName));                                                  \
    BT_NODE(ret)->vtbl = __tname##_vtbl_inst();                                                    \
    return ret;                                                                                    \
  }

void     bt_node_del(bt_Node* node);
bt_Node* bt_node_init(bt_Node* self);
void     bt_node_fini(bt_Node* self);
void     bt_node_abort(bt_Node* self, Ecs*, ecs_entity_t);
void     bt_node_finish(bt_Node* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);

typedef struct bt_Root
{
  BT_EXTEND_NODE(bt_Node)
  bt_Node* child;
} bt_Root;

void      bt_root_vtbl_init(bt_NodeVtbl* vtbl);
bt_Root*  bt_root_new();
void      bt_root_fini(bt_Root* self);
bt_Status bt_root_exec(bt_Root* self, Ecs* ecs, ecs_entity_t entity);
void      bt_root_set_child(bt_Root* self, bt_Node* node);

/*==COMPOSITE==*/

typedef struct bt_Selector
{
  BT_EXTEND_NODE(bt_Node)
  PtrArray* children;
  s32       curr;
} bt_Selector;

const bt_NodeVtbl* bt_selector_vtbl_inst();
void               bt_selector_vtbl_init(bt_NodeVtbl* vtbl);

bt_Selector* bt_selector_new();
bt_Selector* bt_selector_init(bt_Selector* self);
void         bt_selector_fini(bt_Selector* self);
void         bt_selector_abort(bt_Selector* self, Ecs* ecs, ecs_entity_t entity);
void bt_selector_finish(bt_Selector* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);
bt_Status bt_selector_exec(bt_Selector* self, Ecs* ecs, ecs_entity_t entity);
void      bt_selector_add(bt_Selector* self, bt_Node* node);

typedef struct bt_Sequence
{
  BT_EXTEND_NODE(bt_Node)
  PtrArray* children;
  s32       curr;
} bt_Sequence;

const bt_NodeVtbl* bt_sequence_vtbl_inst();
void               bt_sequence_vtbl_init(bt_NodeVtbl* vtbl);
bt_Sequence*       bt_sequence_new();
bt_Sequence*       bt_sequence_init(bt_Sequence* self);
void               bt_sequence_fini(bt_Sequence* self);
void bt_sequence_finish(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);
void bt_sequence_abort(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity);
bt_Status bt_sequence_exec(bt_Sequence* self, Ecs* ecs, ecs_entity_t entity);
void      bt_sequence_add(bt_Sequence* self, bt_Node* node);

/*==DECORATOR==*/

typedef struct bt_Decorator
{
  BT_EXTEND_NODE(bt_Node)
  bt_Node* child;
  BOOL     is_child_running;
} bt_Decorator;

typedef struct bt_DecoratorVtbl
{
  BT_EXTEND_VTBL(bt_NodeVtbl)
} bt_DecoratorVtbl;

const bt_NodeVtbl* bt_decorator_vtbl_inst();
void               bt_decorator_vtbl_init(bt_DecoratorVtbl* vtbl);
bt_Decorator*      bt_decorator_init(bt_Decorator* self);
void               bt_decorator_fini(bt_Decorator* self);
void               bt_decorator_abort(bt_Decorator* self, Ecs* ecs, ecs_entity_t entity);
void               bt_decorator_set_child(bt_Decorator* self, bt_Node* node);
bt_Node*           bt_decorator_steal_child(bt_Decorator* self);
void               bt_decorator_del_child(bt_Decorator* self);

typedef struct bt_Repeater
{
  BT_EXTEND_NODE(bt_Decorator)
  int original_times;
  int times;
} bt_Repeater;

typedef struct bt_RepeaterVtbl
{
  BT_EXTEND_VTBL(bt_DecoratorVtbl)
} bt_RepeaterVtbl;

const bt_NodeVtbl* bt_repeater_vtbl_inst();
void               bt_repeater_vtbl_init(bt_RepeaterVtbl* vtbl);
bt_Repeater*       bt_repeater_new(int times);
void bt_repeater_finish(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);
void bt_repeater_abort(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity);
bt_Status bt_repeater_exec(bt_Repeater* self, Ecs* ecs, ecs_entity_t entity);

typedef struct bt_Inverter
{
  BT_EXTEND_NODE(bt_Decorator)
} bt_Inverter;

typedef struct bt_InverterVtbl
{
  BT_EXTEND_VTBL(bt_DecoratorVtbl)
} bt_InverterVtbl;

const bt_NodeVtbl* bt_inverter_vtbl_inst();
void               bt_inverter_vtbl_init(bt_InverterVtbl* vtbl);
bt_Inverter*       bt_inverter_new();
void               bt_inverter_abort(bt_Inverter* self, Ecs* ecs, ecs_entity_t entity);
bt_Status          bt_inverter_exec(bt_Inverter* self, Ecs* ecs, ecs_entity_t entity);

typedef struct bt_Condition bt_Condition;
typedef BOOL (*bt_pred_fn_t)(bt_Condition* self, Ecs* ecs, ecs_entity_t entity);

struct bt_Condition
{
  BT_EXTEND_NODE(bt_Decorator)
  BOOL is_child_running;
  BOOL abort_if_fail;
};

typedef struct bt_ConditionVtbl
{
  BT_EXTEND_VTBL(bt_NodeVtbl)
  bt_pred_fn_t pred;
} bt_ConditionVtbl;

#define BT_CONDITION(__con) ((bt_Condition*)__con)
#define BT_CONDITION_VTBL(__vtbl) ((bt_ConditionVtbl*)__vtbl)
#define bt_condition_vc_pred(__self, __ecs, __entity)                                              \
  BT_CONDITION_VTBL(BT_GET_VTBL(__self))->pred(__self, __ecs, __entity)

const bt_NodeVtbl* bt_condition_vtbl_inst();
void               bt_condition_vtbl_init(bt_ConditionVtbl* vtbl);
bt_Condition*      bt_condition_init(bt_Condition* self, BOOL abort_if_fail);
bt_Status          bt_condition_exec(bt_Condition* self, Ecs* ecs, ecs_entity_t entity);

#endif // BEHAVIOUR_TREE_H
