#ifndef BEHAVIOUR_TREE_H
#define BEHAVIOUR_TREE_H

#define BT_MAX_CHILD 10

typedef enum
{
  BT_NODE_TYPE_SELECTOR,
  BT_NODE_TYPE_SEQUENCE,
  BT_NODE_TYPE_LEAF,
} bt_NodeType;

typedef enum
{
  BT_STATUS_SUCCESS,
  BT_STATUS_FAILURE,
  BT_STATUS_RUNNING,
} bt_status;

typedef struct
{
  int node[BT_MAX_CHILD];
  int cnt;
} bt_NodeList;

typedef struct
{
  bt_NodeType type;
  bt_NodeList child;
} bt_Node;

typedef struct
{
  bt_Node* mem_pool;
  
} bt_BehaviourTree;

#endif // BEHAVIOUR_TREE_H
