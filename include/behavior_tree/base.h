#ifndef BEHAVIOR_TREE
#define BEHAVIOR_TREE

#include "blackboard.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} BTStatus;

typedef struct _BTNode BTNode;

typedef struct
{
  Ecs*         registry;
  Blackboard*  blackboard;
  ecs_entity_t entity;
} BTUpdateContext;

typedef void (*BTFinalizeFunc)(BTNode*);
typedef BTStatus (*BTOnTickFunc)(BTNode*, const BTUpdateContext*);
typedef void (*BTOnFinishFunc)(BTNode*, const BTUpdateContext*);
typedef void (*BTOnStartFunc)(BTNode*, const BTUpdateContext*);

typedef struct BTNodeVtbl
{
  size_t         size;
  BTFinalizeFunc fini;
  BTOnStartFunc  start;
  BTOnTickFunc   tick;
  BTOnFinishFunc finish;
} BTNodeVtbl;

#define BT_VTBL_INITIALIZER(T, Base, base, ...)                                                    \
  static void vtbl_init(T##Vtbl* vtbl)                                                             \
  {                                                                                                \
    base##_vtbl_init((Base##Vtbl*)vtbl);                                                           \
    BT_NODE_VTBL(vtbl)->size = sizeof(T);                                                          \
    __VA_ARGS__                                                                                    \
  }                                                                                                \
  static const BTNodeVtbl* vtbl_inst()                                                             \
  {                                                                                                \
    static T##Vtbl vtbl;                                                                           \
    static BOOL    initialized = FALSE;                                                            \
    if (!initialized)                                                                              \
    {                                                                                              \
      vtbl_init(&vtbl);                                                                            \
      initialized = TRUE;                                                                          \
    }                                                                                              \
    return (BTNodeVtbl*)&vtbl;                                                                     \
  }

#define BT_INST_DECL(T, Base, ...)                                                                 \
  typedef struct _##T                                                                              \
  {                                                                                                \
    Base _base;                                                                                    \
    struct __VA_ARGS__;                                                                            \
  } T;

#define BT_INST_DECL_EMPTY(T, Base)                                                                \
  typedef struct _##T                                                                              \
  {                                                                                                \
    Base _base;                                                                                    \
  } T;

#define BT_VTBL_DECL(T, Base, ...)                                                                 \
  typedef struct _##T##Vtbl                                                                        \
  {                                                                                                \
    Base##Vtbl _base;                                                                              \
    struct __VA_ARGS__;                                                                            \
  } T##Vtbl;

#define BT_VTBL_DECL_EMPTY(T, Base)                                                                \
  typedef struct _##T##Vtbl                                                                        \
  {                                                                                                \
    Base##Vtbl _base;                                                                              \
  } T##Vtbl;

#define BT_GLOBAL_VTBL_INITIALIZER_DECL(T, t) void t##_vtbl_init(T##Vtbl* vtbl);

#define BT_GLOBAL_VTBL_INITIALIZER_IMPL(T, t, B, b, ...)                                           \
  void t##_vtbl_init(T##Vtbl* vtbl)                                                                \
  {                                                                                                \
    b##_vtbl_init((B##Vtbl*)vtbl);                                                                 \
    BT_NODE_VTBL(vtbl)->size = sizeof(T);                                                          \
    __VA_ARGS__                                                                                    \
  }

void bt_node_vtbl_init(BTNodeVtbl* vtbl);

struct _BTNode
{
  const BTNodeVtbl* vtbl;
  BTNode*           parent;
};

void* bt_alloc(const BTNodeVtbl* vtbl);

BTNode*  bt_node_init(BTNode* self);
void     bt_node_fini(BTNode* self);
void     bt_node_on_finish(BTNode* self, const BTUpdateContext* ctx, BOOL succeed);
void     bt_node_on_start(BTNode* self, const BTUpdateContext* ctx);
BTStatus bt_node_on_tick(BTNode* self, const BTUpdateContext* ctx);

#define BT_NODE(ptr) ((BTNode*)ptr)
#define BT_NODE_VTBL(ptr) ((BTNodeVtbl*)ptr)
#define BT_GET_VTBL(self) (BT_NODE(self)->vtbl)
#define BT_SET_VTBL(node, _vtbl) (BT_NODE(node)->vtbl = _vtbl)

#define bt_node_tick(self, ctx) (BT_GET_VTBL(self)->tick(BT_NODE(self), ctx))
#define bt_node_finish(self, ctx) (BT_GET_VTBL(self)->finish(BT_NODE(self), ctx))
#define bt_node_start(self, ctx) (BT_GET_VTBL(self)->start(BT_NODE(self), ctx))

void    bt_node_del(BTNode* node);
BTNode* bt_node_init(BTNode* self);

typedef struct _BTRoot BTRoot;
BTRoot*                bt_root_new(BTNode* child);

typedef BTNodeVtbl BTCompositeNodeVtbl;
BT_INST_DECL(BTCompositeNode, BTNode, { PtrArray* children; })
BT_GLOBAL_VTBL_INITIALIZER_DECL(BTCompositeNode, bt_composite_node)
#define BT_COMPOSITE_NODE(ptr) ((BTCompositeNode*)ptr)
BTCompositeNode* bt_composite_node_init(BTCompositeNode* self);
void             bt_composite_node_fini(BTCompositeNode* self);
void             bt_composite_node_add(BTCompositeNode* self, BTNode* node);
BTNode*          bt_composite_node_child_at(BTCompositeNode* self, int index);

typedef BTNodeVtbl BTDecoratorVtbl;
BT_INST_DECL(BTDecorator, BTNode, {
  BTNode* child;
  BOOL    isChildRunning;
})
#define BT_DECORATOR(ptr) ((BTDecorator*)ptr)
BT_GLOBAL_VTBL_INITIALIZER_DECL(BTDecorator, bt_decorator)

BTDecorator* bt_decorator_init(BTDecorator* self);
void         bt_decorator_fini(BTDecorator* self);
void         bt_decorator_on_finish(BTDecorator* self, const BTUpdateContext* ctx);
BTStatus     bt_decorator_on_tick(BTDecorator* self, const BTUpdateContext* ctx);
void         bt_decorator_set_child(BTDecorator* self, BTNode* node);
BTNode*      bt_decorator_steal_child(BTDecorator* self);

#endif // BEHAVIOR_TREE
