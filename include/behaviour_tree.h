#ifndef BEHAVIOUR_TREE_H
#define BEHAVIOUR_TREE_H

#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} BTStatus;

typedef struct BTNode BTNode;

typedef BTStatus (*BTExecuteFunc)(BTNode*, Ecs*, ecs_entity_t);
typedef void (*BTAbortFunc)(BTNode*, Ecs* ecs, ecs_entity_t);
typedef void (*BTFinishFunc)(BTNode*, Ecs* ecs, ecs_entity_t, BOOL succeed);
typedef destroy_fn_t BTFinalizeFunc;

typedef struct BTNodeVtbl
{
  destroy_fn_t  fini;
  BTExecuteFunc exec;
  BTAbortFunc   abort;
  BTFinishFunc  finish;
} BTNodeVtbl;

#define BT_PRIVATE_NODE(T)                                                                         \
  static void              VtblInit(T##Vtbl* vtbl);                                                \
  static const BTNodeVtbl* VtblInst()                                                              \
  {                                                                                                \
    static T##Vtbl vtbl;                                                                           \
    static BOOL    initialized = FALSE;                                                            \
    if (!initialized)                                                                              \
    {                                                                                              \
      VtblInit(&vtbl);                                                                             \
      initialized = TRUE;                                                                          \
    }                                                                                              \
    return (BTNodeVtbl*)&vtbl;                                                                     \
  }                                                                                                \
  INLINE T* Alloc()                                                                                \
  {                                                                                                \
    T* self             = SDL_malloc(sizeof(T));                                                   \
    BT_NODE(self)->vtbl = VtblInst();                                                              \
    return self;                                                                                   \
  }

#define BT_PUBLIC_NODE_DECLARE(T, t)                                                               \
  void              t##_vtbl_init(T##Vtbl* vtbl);                                                  \
  const BTNodeVtbl* t##_vtbl_inst();                                                               \
  T*                t##_alloc();

#define BT_PUBLIC_NODE_IMPLEMENT(T, t)                                                             \
  void              t##_vtbl_init(T##Vtbl* vtbl);                                                  \
  const BTNodeVtbl* t##_vtbl_inst()                                                                \
  {                                                                                                \
    static T##Vtbl vtbl;                                                                           \
    static BOOL    initialized = FALSE;                                                            \
    if (!initialized)                                                                              \
    {                                                                                              \
      t##_vtbl_init(&vtbl);                                                                        \
      initialized = TRUE;                                                                          \
    }                                                                                              \
    return (BTNodeVtbl*)&vtbl;                                                                     \
  }                                                                                                \
  T* t##_alloc()                                                                                   \
  {                                                                                                \
    T* ret             = SDL_malloc(sizeof(T));                                                    \
    BT_NODE(ret)->vtbl = t##_vtbl_inst();                                                          \
    return ret;                                                                                    \
  }

void bt_node_vtbl_init(BTNodeVtbl* vtbl);

struct BTNode
{
  const BTNodeVtbl* vtbl;
  BTNode*           parent;
};

#define BT_EXTEND_NODE(__base) __base __parent_inst;
#define BT_EXTEND_VTBL(__bvtbl) __bvtbl __parent_vtbl;
#define BT_NODE(self) ((BTNode*)self)
#define BT_GET_VTBL(self) (BT_NODE(self)->vtbl)

/**
 * execute this node
 */
#define bt_node_exec(self, registry, entity)                                                       \
  (BT_NODE(self)->vtbl->exec(BT_NODE(self), registry, entity))

/**
 * abort this node
 */
#define bt_node_abort(self, registry, entity)                                                      \
  (BT_NODE(self)->vtbl->abort(BT_NODE(self), registry, entity))

/**
 * called when this node is finished
 */
#define bt_node_finish(self, registry, entity, succeed)                                            \
  (BT_NODE(self)->vtbl->finish(BT_NODE(self), registry, entity, succeed))

void    bt_node_del(BTNode* node);
BTNode* bt_node_init(BTNode* self);
void    bt_node_real_fini(BTNode* self);
void    bt_node_real_abort(BTNode* self, Ecs*, ecs_entity_t);
void    bt_node_real_finish(BTNode* self, Ecs* ecs, ecs_entity_t entity, BOOL succeed);

typedef struct _BTRoot BTRoot;
BTRoot*                bt_root_new(BTNode* child);

/*==COMPOSITE==*/

typedef struct _BTCompositeNode
{
  BTNode    _base;
  PtrArray* children;
} BTCompositeNode;

typedef struct _BTCompositeNodeVtbl
{
  BTNodeVtbl _base;
} BTCompositeNodeVtbl;
#define BT_COMPOSITE_NODE(ptr) ((BTCompositeNode*)ptr)

BT_PUBLIC_NODE_DECLARE(BTCompositeNode, bt_composite_node)
BTCompositeNode* bt_composite_node_init(BTCompositeNode* self);
void             bt_composite_node_fini(BTCompositeNode* self);
void             bt_composite_node_add(BTCompositeNode* self, BTNode* node);
BTNode*          bt_composite_node_child_at(BTCompositeNode* self, int index);

BTCompositeNode* bt_selector_new();
BTCompositeNode* bt_sequence_new();

/*==DECORATOR==*/
typedef struct
{
  BTNode  _base;
  BTNode* child;
  BOOL    is_child_running;
} BTDecorator;

typedef struct
{
  BTNodeVtbl _base;
} BTDecoratorVtbl;

BT_PUBLIC_NODE_DECLARE(BTDecorator, bt_decorator)
#define BT_DECORATOR(ptr) ((BTDecorator*)ptr)

BTDecorator* bt_decorator_init(BTDecorator* self);
void         bt_decorator_fini(BTDecorator* self);
void         bt_decorator_abort(BTDecorator* self, Ecs* ecs, ecs_entity_t entity);
void         bt_decorator_set_child(BTDecorator* self, BTNode* node);
BTNode*      bt_decorator_steal_child(BTDecorator* self);

BTDecorator* bt_repeater_new(int times);
BTDecorator* bt_inverter_new();

typedef struct BTCondition BTCondition;
typedef BOOL (*bt_pred_fn_t)(BTCondition* self, Ecs* ecs, ecs_entity_t entity);

struct BTCondition
{
  BTDecorator _base;
  BOOL        is_child_running;
  BOOL        abort_if_fail;
  BOOL        invert_result;
};

typedef struct
{
  BTNodeVtbl   _base;
  bt_pred_fn_t pred;
} BTConditionVtbl;

#define BT_CONDITION(ptr) ((bt_Condition*)ptr)
#define BT_CONDITION_VTBL(ptr) ((BTConditionVtbl*)ptr)
#define bt_condition_pred(self, registry, entity)                                                  \
  BT_CONDITION_VTBL(BT_GET_VTBL(self))->pred(self, registry, entity)

BT_PUBLIC_NODE_DECLARE(BTCondition, bt_condition)
BTCondition* bt_condition_init(BTCondition* self, BOOL abort_if_fail, BOOL invert_result);

#endif // BEHAVIOUR_TREE_H
