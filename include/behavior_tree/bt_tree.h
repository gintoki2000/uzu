// ngotrung Sun 30 May 2021 08:19:23 AM +07
#ifndef BT_TREE_H
#define BT_TREE_H
#include "behavior_tree/bt_node.h"

typedef struct bt_BBKeySet bt_BBKeySet;

#define BT_BBKEY_NULL UINT32_MAX

bt_BBKeySet* bt_bbkey_set_create(void);
void         bt_bbkey_set_free(bt_BBKeySet* keySet);
BOOL         bt_bbkey_set_add(bt_BBKeySet* keySet, const char* key);
BOOL         bt_get_bbkey_index(bt_BBKeySet* keySet, const char* key, u32* outIndex);
const char*  bt_get_bbkey_name(bt_BBKeySet* keySet, u32 index);
u32          bt_bbkey_set_count(bt_BBKeySet* keySet);

typedef struct bt_Tree bt_Tree;
typedef struct bt_Root bt_Root;

bt_Tree*     bt_tree_create(void);
void         bt_tree_free(bt_Tree* tree);
void         bt_tree_tick(bt_Tree* self, const bt_UpdateContext*);
u32          bt_tree_node_count(bt_Tree* self);
void         bt_tree_set_root(bt_Tree* self, bt_Root* root);
bt_Root*     bt_tree_root(bt_Tree* self);
void         bt_tree_set_key_set(bt_Tree* self, bt_BBKeySet* keySet);
bt_BBKeySet* bt_tree_key_set(bt_Tree* self);
#endif // BT_TREE_H
