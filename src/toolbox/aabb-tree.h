#ifndef AABB_TREE_H
#define AABB_TREE_H
#include <common.h>

typedef struct AABBTree AABBTree;

AABBTree* aabb_tree_new();
void      aabb_tree_delete(AABBTree* AABBTree);

int  aabb_tree_create_proxy(AABBTree* AABBTree, void* user_data, const AABB* aabb);
void aabb_tree_destroy_proxy(AABBTree* AABBTree, int proxy_id);
bool aabb_tree_move_proxy(AABBTree*   AABBTree,
                          int         proxy_id,
                          const AABB* aabb,
                          const Vec2  displacement);
void aabb_tree_query(AABBTree* AABBTree, const AABB* aabb, Callback cb);

void* aabb_tree_get_user_data(AABBTree* AABBTree, int proxy_id);

const AABB* aabb_tree_get_fat_aabb(AABBTree* AABBTree, int proxy_id);

#endif // AABB_TREE_H
