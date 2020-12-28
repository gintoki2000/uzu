#ifndef BEHAVIOUR_TREE_H
#define BEHAVIOUR_TREE_H

#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} BTStatus;

typedef struct BTNode BTNode;

typedef BTStatus (*bt_exec_fn_t)(BTNode*, Ecs*, ecs_entity_t);
typedef void (*bt_abort_fn_t)(BTNode*, Ecs* ecs, ecs_entity_t);
typedef void (*bt_finish_fn_t)(BTNode*, Ecs* ecs, ecs_entity_t, BTStatus finish_status);

typedef struct BTNodeVtbl
{
  const struct BTNodeVtbl* parent;
  destroy_fn_t             fini;
  bt_exec_fn_t             exec;
  bt_abort_fn_t            abort;
  bt_finish_fn_t           finish;
} BTNodeVtbl;

#define BT_VTBL_INST_FN(__TName, __tname)                                                          \
  const BTNodeVtbl* __tname##_vtbl_inst()                                                          \
  {                                                                                                \
    static __TName##Vtbl vtbl;                                                                     \
    static BOOL          initialized = FALSE;                                                      \
    if (!initialized)                                                                              \
    {                                                                                              \
      __tname##_vtbl_init(&vtbl);                                                                  \
      initialized = TRUE;                                                                          \
    }                                                                                              \
    return (BTNodeVtbl*)&vtbl;                                                                     \
  }

const BTNodeVtbl* bt_node_vtbl_inst();
void              bt_node_vtbl_init(BTNodeVtbl* vtbl);

struct BTNode
{
  const BTNodeVtbl* vtbl;
  BTNode*           parent;
};

#define BT_EXTEND_NODE(__base) __base __parent_inst;
#define BT_EXTEND_VTBL(__bvtbl) __bvtbl __parent_vtbl;
#define BT_NODE(__node) ((BTNode*)__node)
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

void    bt_node_del(BTNode* node);
BTNode* bt_node_init(BTNode* self);
void    bt_node_fini(BTNode* self);
void    bt_node_abort(BTNode* self, Ecs*, ecs_entity_t);
void    bt_node_finish(BTNode* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);

typedef struct
{
  BT_EXTEND_NODE(BTNode)
  BTNode* child;
} BTRoot;

void     bt_root_vtbl_init(BTNodeVtbl* vtbl);
BTRoot*  bt_root_new();
void     bt_root_fini(BTRoot* self);
BTStatus bt_root_exec(BTRoot* self, Ecs* ecs, ecs_entity_t entity);
void     bt_root_set_child(BTRoot* self, BTNode* node);

/*==COMPOSITE==*/

typedef struct bt_Selector
{
  BT_EXTEND_NODE(BTNode)
  PtrArray* children;
  s32       curr;
} BTSelector;

const BTNodeVtbl* bt_selector_vtbl_inst();
void              bt_selector_vtbl_init(BTNodeVtbl* vtbl);

BTSelector* bt_selector_new();
BTSelector* bt_selector_init(BTSelector* self);
void        bt_selector_fini(BTSelector* self);
void        bt_selector_abort(BTSelector* self, Ecs* ecs, ecs_entity_t entity);
void bt_selector_finish(BTSelector* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
BTStatus bt_selector_exec(BTSelector* self, Ecs* ecs, ecs_entity_t entity);
void     bt_selector_add(BTSelector* self, BTNode* node);

typedef struct
{
  BT_EXTEND_NODE(BTNode)
  PtrArray* children;
  s32       curr;
} BTSequence;

const BTNodeVtbl* bt_sequence_vtbl_inst();
void              bt_sequence_vtbl_init(BTNodeVtbl* vtbl);
BTSequence*       bt_sequence_new();
BTSequence*       bt_sequence_init(BTSequence* self);
void              bt_sequence_fini(BTSequence* self);
void bt_sequence_finish(BTSequence* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
void bt_sequence_abort(BTSequence* self, Ecs* ecs, ecs_entity_t entity);
BTStatus bt_sequence_exec(BTSequence* self, Ecs* ecs, ecs_entity_t entity);
void     bt_sequence_add(BTSequence* self, BTNode* node);

/*==DECORATOR==*/
typedef struct
{
  BT_EXTEND_NODE(BTNode)
  BTNode* child;
  BOOL    is_child_running;
} BTDecorator;

typedef struct
{
  BT_EXTEND_VTBL(BTNodeVtbl)
} BTDecoratorVtbl;

const BTNodeVtbl* bt_decorator_vtbl_inst();
void              bt_decorator_vtbl_init(BTDecoratorVtbl* vtbl);
BTDecorator*      bt_decorator_init(BTDecorator* self);
void              bt_decorator_fini(BTDecorator* self);
void              bt_decorator_abort(BTDecorator* self, Ecs* ecs, ecs_entity_t entity);
void              bt_decorator_set_child(BTDecorator* self, BTNode* node);
BTNode*           bt_decorator_steal_child(BTDecorator* self);
void              bt_decorator_del_child(BTDecorator* self);

typedef struct
{
  BT_EXTEND_NODE(BTDecorator)
  int original_times;
  int times;
} BTRepeater;

typedef struct
{
  BT_EXTEND_VTBL(BTDecoratorVtbl)
} BTRepeaterVtbl;

const BTNodeVtbl* bt_repeater_vtbl_inst();
void              bt_repeater_vtbl_init(BTRepeaterVtbl* vtbl);
BTRepeater*       bt_repeater_new(int times);
void bt_repeater_finish(BTRepeater* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
void bt_repeater_abort(BTRepeater* self, Ecs* ecs, ecs_entity_t entity);
BTStatus bt_repeater_exec(BTRepeater* self, Ecs* ecs, ecs_entity_t entity);

typedef struct
{
  BT_EXTEND_NODE(BTDecorator)
} BTInverter;

typedef struct
{
  BT_EXTEND_VTBL(BTDecoratorVtbl)
} BTInverterVtbl;

const BTNodeVtbl* bt_inverter_vtbl_inst();
void              bt_inverter_vtbl_init(BTInverterVtbl* vtbl);
BTInverter*       bt_inverter_new();
void              bt_inverter_abort(BTInverter* self, Ecs* ecs, ecs_entity_t entity);
BTStatus          bt_inverter_exec(BTInverter* self, Ecs* ecs, ecs_entity_t entity);

typedef struct BTCondition BTCondition;
typedef BOOL (*bt_pred_fn_t)(BTCondition* self, Ecs* ecs, ecs_entity_t entity);

struct BTCondition
{
  BT_EXTEND_NODE(BTDecorator)
  BOOL is_child_running;
  BOOL abort_if_fail;
};

typedef struct
{
  BT_EXTEND_VTBL(BTNodeVtbl)
  bt_pred_fn_t pred;
} BTConditionVtbl;

#define BT_CONDITION(__con) ((bt_Condition*)__con)
#define BT_CONDITION_VTBL(__vtbl) ((BTConditionVtbl*)__vtbl)
#define bt_condition_vc_pred(__self, __ecs, __entity)                                              \
  BT_CONDITION_VTBL(BT_GET_VTBL(__self))->pred(__self, __ecs, __entity)

const BTNodeVtbl* bt_condition_vtbl_inst();
void              bt_condition_vtbl_init(BTConditionVtbl* vtbl);
BTCondition*      bt_condition_init(BTCondition* self, BOOL abort_if_fail);
BTStatus          bt_condition_exec(BTCondition* self, Ecs* ecs, ecs_entity_t entity);

#endif // BEHAVIOUR_TREE_H
