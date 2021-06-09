// ngotrung Sun 30 May 2021 08:02:00 AM +07
#ifndef BT_NODE_H
#define BT_NODE_H
#include "behavior_tree/bt_runtime.h"
#include "blackboard.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} bt_Status;

typedef enum
{
  BT_NODE_TYPE_ROOT,
  BT_NODE_TYPE_COMPOSITE,
  BT_NODE_TYPE_DECORATOR,
  BT_NODE_TYPE_TASK,
} bt_NodeType;

typedef struct {
  ecs_Registry *registry;
  Blackboard *blackboard;
  bt_RuntimeDataManager *runtimeDataManager;
  ecs_entity_t entity;
} bt_UpdateContext;

typedef void (*bt_FinalizeFunc)(struct bt_Node*);
typedef bt_Status (*bt_OnTickFunc)(void* self, const bt_UpdateContext*);
typedef void (*bt_OnFinishFunc)(void* self, const bt_UpdateContext*);
typedef void (*bt_OnStartFunc)(void* self, const bt_UpdateContext*);
typedef void (*bt_HaltFunc)(void* self, const bt_UpdateContext*);
typedef BOOL (*bt_VisitFunc)(void* arg, struct bt_Node* node);
typedef void (*bt_AcceptFunc)(void* self, void* arg, bt_VisitFunc callback);

typedef struct bt_Node
{
  // virtual methods
  bt_FinalizeFunc fini;
  bt_OnStartFunc  start;
  bt_OnTickFunc   tick;
  bt_OnFinishFunc finish;
  bt_AcceptFunc   accept;

  bt_NodeType     type;
  struct bt_Node* parent;
  struct bt_Tree* tree;
  u32             id;
} bt_Node;

#define BT_NODE_DECL_EMPTY(name, baseClass)                                                        \
  typedef struct name                                                                              \
  {                                                                                                \
    baseClass _base;                                                                               \
  } name;

#define BT_NODE_DECL(name, baseClass, ...)                                                         \
  typedef struct name                                                                              \
  {                                                                                                \
    baseClass _base;                                                                               \
    struct __VA_ARGS__;                                                                            \
  } name;

bt_Node*  bt_node_init(bt_Node* self, bt_NodeType type);
void      bt_node_fini(bt_Node* self);
void      bt_node_on_finish(void* self, const bt_UpdateContext* ctx);
void      bt_node_on_start(void* self, const bt_UpdateContext* ctx);
bt_Status bt_node_on_tick(void* self, const bt_UpdateContext* ctx);
void      bt_node_real_accept(void* self, void* arg, bt_VisitFunc callback);

#define BT_NODE(ptr) ((bt_Node*)ptr)

#define bt_node_tick(self, ctx) (BT_NODE(self)->tick(BT_NODE(self), ctx))
#define bt_node_finish(self, ctx) (BT_NODE(self)->finish(BT_NODE(self), ctx))
#define bt_node_start(self, ctx) (BT_NODE(self)->start(BT_NODE(self), ctx))
#define bt_node_accept(self, arg, callback) (BT_NODE(self)->accept(self, arg, callback))
#define bt_node_get_type(self) (BT_NODE(self)->type)
#define bt_node_get_parent(self) (BT_NODE(self)->parent)
#define bt_node_get_tree(self)(BT_NODE(self)->tree)
#define bt_node_get_id(self) (BT_NODE(self)->id)

void bt_node_free(bt_Node* node);
#endif // BT_NODE_H
