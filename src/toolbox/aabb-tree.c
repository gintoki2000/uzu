
#include <toolbox/aabb-tree.h>
#include <common.h>
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define NULL_NODE (-1)

typedef struct
{
  AABB  aabb;
  void* user_data;
  int   parent;
  int   child1;
  int   child2;
  int   height;
} TreeNode;

struct AABBTree
{
  int          capacity;
  int          count;
  TreeNode* nodes;
  int          root;
  int          free_index;
  int*         next;
};

static AABBTree* aabb_tree_init(AABBTree* AABBTree)
{
  int n = 16;
  AABBTree->capacity = n;
  AABBTree->count = 0;
  AABBTree->root = NULL_NODE;
  AABBTree->nodes = calloc(n, sizeof(TreeNode));
  AABBTree->next = calloc(n, sizeof(int));
  for (int i = 0; i < n; ++i)
  {
    AABBTree->next[i] = i + 1;
    AABBTree->nodes[i].height = -1;
  }
  AABBTree->free_index = 0;
  AABBTree->next[n - 1] = NULL_NODE;
  return AABBTree;
}

static bool is_leaf(const TreeNode* node) { return node->child1 == -1; }

static void aabb_tree_finalize(AABBTree* AABBTree)
{
  free(AABBTree->nodes);
  free(AABBTree->next);
}
static void aabb_tree_grow_if_need(AABBTree* AABBTree)
{
  if (AABBTree->count == AABBTree->capacity)
  {
    assert(AABBTree->free_index == NULL_NODE);
    int          count = AABBTree->count;
    int          new_capacity = AABBTree->capacity * 2;
    TreeNode* nodes = malloc(new_capacity * sizeof(TreeNode));
    AABBTree->nodes = memcpy(nodes, AABBTree->nodes, count * sizeof(TreeNode));
    int* next = malloc(new_capacity * sizeof(int));
    AABBTree->next = memcpy(next, AABBTree->next, count * sizeof(int));
    for (int i = count; i < new_capacity; ++i)
    {
      next[i] = i + 1;
      nodes[i].height = -1;
    }
    next[new_capacity - 1] = -1;
    nodes[new_capacity - 1].height = -1;
    AABBTree->free_index = count;
    AABBTree->capacity = new_capacity;
  }
}

static int aabb_tree_allocate_node(AABBTree* AABBTree)
{
  aabb_tree_grow_if_need(AABBTree);
  TreeNode* nodes = AABBTree->nodes;
  int          idx = AABBTree->free_index;
  AABBTree->free_index = AABBTree->next[idx];
  nodes[idx].user_data = NULL;
  nodes[idx].parent = NULL_NODE;
  nodes[idx].child1 = NULL_NODE;
  nodes[idx].child2 = NULL_NODE;
  nodes[idx].height = 0;
  ++AABBTree->count;
  return idx;
}

static void aabb_tree_free_node(AABBTree* AABBTree, int node_idx)
{
  assert(node_idx >= 0 && node_idx < AABBTree->capacity);
  assert(AABBTree->count > 0);

  AABBTree->next[node_idx] = AABBTree->free_index;
  AABBTree->free_index = node_idx;
  AABBTree->nodes[node_idx].height = -1;
  --AABBTree->count;
}

static int aabb_tree_balance(AABBTree* AABBTree, int idx)
{
  int ia = idx;
  assert(ia != NULL_NODE);
  TreeNode* nodes = AABBTree->nodes;
  TreeNode* a = &nodes[ia];
  if (is_leaf(a) || a->height < 2)
  {
    return ia;
  }

  int          ib = a->child1;
  int          ic = a->child2;
  TreeNode* b = nodes + ib;
  TreeNode* c = nodes + ic;
  int          balance = c->height - b->height;
  if (balance > 1)
  {
    int          ih = c->child1;
    int          ik = c->child2;
    TreeNode* h = nodes + ih;
    TreeNode* k = nodes + ik;

    c->child1 = ia;
    c->parent = a->parent;
    a->parent = ic;

    if (c->parent != NULL_NODE)
    {
      if (nodes[c->parent].child1 == ia)
      {
        nodes[c->parent].child1 = ic;
      }
      else
      {
        nodes[c->parent].child2 = ic;
      }
    }
    else
    {
      AABBTree->root = ic;
    }

    if (h->height > k->height)
    {
      c->child2 = ih;
      a->child2 = ik;
      k->parent = ia;
      a->aabb = aabb_merge(&b->aabb, &k->aabb);
      c->aabb = aabb_merge(&a->aabb, &h->aabb);
      a->height = 1 + max(b->height, k->height);
      c->height = 1 + max(a->height, h->height);
    }
    else
    {
      c->child2 = ik;
      a->child2 = ih;
      h->parent = ia;
      a->aabb = aabb_merge(&b->aabb, &h->aabb);
      c->aabb = aabb_merge(&a->aabb, &k->aabb);
      a->height = 1 + max(b->height, h->height);
      c->height = 1 + max(a->height, k->height);
    }

    return ic;
  }

  if (balance < -1)
  {
    int          id = b->child1;
    int          ie = b->child2;
    TreeNode* d = nodes + id;
    TreeNode* e = nodes + ie;
    assert(0 <= id && id < AABBTree->capacity);
    assert(0 <= ie && ie < AABBTree->capacity);

    b->child1 = ia;
    b->parent = a->parent;
    a->parent = ib;

    if (b->parent != NULL_NODE)
    {
      if (nodes[b->parent].child1 == ia)
      {
        nodes[b->parent].child1 = ib;
      }
      else
      {
        assert(nodes[b->parent].child2 == ia);
        nodes[b->parent].child2 = ib;
      }
    }
    else
    {
      AABBTree->root = ib;
    }

    if (d->height > e->height)
    {
      b->child2 = id;
      a->child1 = ie;
      e->parent = ia;
      a->aabb = aabb_merge(&c->aabb, &e->aabb);
      b->aabb = aabb_merge(&a->aabb, &d->aabb);

      a->height = 1 + max(c->height, e->height);
      b->height = 1 + max(a->height, d->height);
    }
    else
    {
      b->child2 = ie;
      a->child1 = id;
      d->parent = ia;
      a->aabb = aabb_merge(&c->aabb, &d->aabb);
      b->aabb = aabb_merge(&a->aabb, &e->aabb);

      a->height = 1 + max(c->height, d->height);
      b->height = 1 + max(a->height, e->height);
    }

    return ib;
  }

  return ia;
}

static void aabb_tree_insert_leaf(AABBTree* AABBTree, int leaf)
{
  TreeNode* nodes = AABBTree->nodes;
  if (AABBTree->root == NULL_NODE)
  {
    AABBTree->root = leaf;
    nodes[AABBTree->root].parent = NULL_NODE;
    return;
  }

  AABB leaf_aabb = nodes[leaf].aabb;
  int  index = AABBTree->root;
  while (is_leaf(&nodes[index]) == false)
  {
    int child1 = nodes[index].child1;
    int child2 = nodes[index].child2;

    float area = aabb_premiter(&nodes[index].aabb);

    AABB  combined_aabb = aabb_merge(&nodes[index].aabb, &leaf_aabb);
    float combine_area = aabb_premiter(&combined_aabb);

    float cost = 2.0f * combine_area;

    float inheritance_cost = 2.0f * (combine_area - area);

    float cost1;
    if (is_leaf(&nodes[child1]))
    {
      AABB aabb = aabb_merge(&leaf_aabb, &nodes[child1].aabb);
      cost1 = aabb_premiter(&aabb) + inheritance_cost;
    }
    else
    {
      AABB  aabb = aabb_merge(&leaf_aabb, &nodes[child1].aabb);
      float oldArea = aabb_premiter(&nodes[child1].aabb);
      float newArea = aabb_premiter(&aabb);
      cost1 = (newArea - oldArea) + inheritance_cost;
    }

    float cost2;
    if (is_leaf(&nodes[child2]))
    {
      AABB aabb = aabb_merge(&leaf_aabb, &nodes[child2].aabb);
      cost2 = aabb_premiter(&aabb) + inheritance_cost;
    }
    else
    {
      AABB  aabb = aabb_merge(&leaf_aabb, &nodes[child2].aabb);
      float oldArea = aabb_premiter(&nodes[child2].aabb);
      float newArea = aabb_premiter(&aabb);
      cost2 = newArea - oldArea + inheritance_cost;
    }

    if (cost < cost1 && cost < cost2)
    {
      break;
    }

    if (cost1 < cost2)
    {
      index = child1;
    }
    else
    {
      index = child2;
    }
  }

  int sibling = index;

  // Create a new parent.
  int old_parent = nodes[sibling].parent;
  int new_parent = aabb_tree_allocate_node(AABBTree);
  nodes[new_parent].parent = old_parent;
  nodes[new_parent].user_data = NULL;
  nodes[new_parent].aabb = aabb_merge(&leaf_aabb, &nodes[sibling].aabb);
  nodes[new_parent].height = nodes[sibling].height + 1;

  if (old_parent != NULL_NODE)
  {
    // The sibling was not the root.
    if (nodes[old_parent].child1 == sibling)
    {
      nodes[old_parent].child1 = new_parent;
    }
    else
    {
      nodes[old_parent].child2 = new_parent;
    }

    nodes[new_parent].child1 = sibling;
    nodes[new_parent].child2 = leaf;
    nodes[sibling].parent = new_parent;
    nodes[leaf].parent = new_parent;
  }
  else
  {
    // The sibling was the root.
    nodes[new_parent].child1 = sibling;
    nodes[new_parent].child2 = leaf;
    nodes[sibling].parent = new_parent;
    nodes[leaf].parent = new_parent;
    AABBTree->root = new_parent;
  }

  // Walk back up the tree fixing heights and AABBs
  index = nodes[leaf].parent;
  while (index != NULL_NODE)
  {
    index = aabb_tree_balance(AABBTree, index);

    int child1 = nodes[index].child1;
    int child2 = nodes[index].child2;

    assert(child1 != NULL_NODE);
    assert(child2 != NULL_NODE);

    nodes[index].height = 1 + max(nodes[child1].height, nodes[child2].height);
    nodes[index].aabb = aabb_merge(&nodes[child1].aabb, &nodes[child2].aabb);

    index = nodes[index].parent;
  }
}

static void aabb_tree_remove_leaf(AABBTree* AABBTree, int leaf)
{

  TreeNode* nodes = AABBTree->nodes;
  if (leaf == AABBTree->root)
  {
    AABBTree->root = NULL_NODE;
    return;
  }

  int parent = nodes[leaf].parent;
  int grand_parent = nodes[parent].parent;
  int sibling;
  if (nodes[parent].child1 == leaf)
  {
    sibling = nodes[parent].child2;
  }
  else
  {
    sibling = nodes[parent].child1;
  }

  if (grand_parent != NULL_NODE)
  {
    if (nodes[grand_parent].child1 == parent)
    {
      nodes[grand_parent].child1 = sibling;
    }
    else
    {
      nodes[grand_parent].child2 = sibling;
    }
    nodes[sibling].parent = grand_parent;
    aabb_tree_free_node(AABBTree, parent);

    int index = grand_parent;
    while (index != NULL_NODE)
    {
      index = aabb_tree_balance(AABBTree, index);

      int child1 = nodes[index].child1;
      int child2 = nodes[index].child2;

      nodes[index].aabb = aabb_merge(&nodes[child1].aabb, &nodes[child2].aabb);
      nodes[index].height = 1 + max(nodes[child1].height, nodes[child2].height);

      index = nodes[index].parent;
    }
  }
  else
  {
    AABBTree->root = sibling;
    nodes[sibling].parent = NULL_NODE;
    aabb_tree_free_node(AABBTree, parent);
  }
}

typedef struct
{
  int  capacity;
  int  count;
  int* items;
} Stack;

static void stack_init(Stack* s)
{
  int n = 64;
  s->capacity = n;
  s->count = 0;
  s->items = calloc(n, sizeof(int));
}

static void stack_finalize(Stack* s) { free(s->items); }

static void stack_grow(Stack* s)
{
  s->capacity *= 2;
  s->items = realloc(s->items, s->capacity * sizeof(int));
}
static void stack_push(Stack* s, int v)
{
  if (s->count == s->capacity)
  {
    stack_grow(s);
  }

  s->items[s->count++] = v;
}
static int stack_pop(Stack* s)
{
  assert(s->count > 0 && "stack empty");
  return s->items[--s->count];
}
static bool stack_empty(Stack* s) { return s->count == 0; }

/********************************************************************/

AABBTree* aabb_tree_new() { return aabb_tree_init(malloc(sizeof(AABBTree))); }

void aabb_tree_delete(AABBTree* AABBTree)
{
  aabb_tree_finalize(AABBTree);
  free(AABBTree);
}

int aabb_tree_create_proxy(AABBTree* AABBTree, void* user_data, const AABB* aabb)
{
  int          idx = aabb_tree_allocate_node(AABBTree);
  TreeNode* nodes = AABBTree->nodes;

  nodes[idx].aabb = *aabb;
  nodes[idx].parent = NULL_NODE;
  nodes[idx].child1 = NULL_NODE;
  nodes[idx].child2 = NULL_NODE;
  nodes[idx].height = 0;
  nodes[idx].user_data = user_data;

  aabb_extend(&nodes[idx].aabb, 5.f);

  aabb_tree_insert_leaf(AABBTree, idx);

  return idx;
}

void aabb_tree_destroy_proxy(AABBTree* AABBTree, int proxy_id)
{
  assert(0 <= proxy_id && proxy_id < AABBTree->capacity);
  assert(is_leaf(&AABBTree->nodes[proxy_id]));

  aabb_tree_remove_leaf(AABBTree, proxy_id);
  aabb_tree_free_node(AABBTree, proxy_id);
}

bool aabb_tree_move_proxy(AABBTree*   AABBTree,
                          int         proxy_id,
                          const AABB* aabb,
                          const Vec2  displacement)
{
  TreeNode* nodes = AABBTree->nodes;
  assert(0 <= proxy_id && proxy_id < AABBTree->capacity);

  assert(is_leaf(&nodes[proxy_id]));

  AABB fat_aabb = *aabb;
  aabb_extend(&fat_aabb, 5.f);

  Vec2 d = mulv(displacement, 2.f);

  if (d.x < 0.0f)
  {
    fat_aabb.lower_bound.x += d.x;
  }
  else
  {
    fat_aabb.upper_bound.x += d.x;
  }

  if (d.y < 0.0f)
  {
    fat_aabb.lower_bound.y += d.y;
  }
  else
  {
    fat_aabb.upper_bound.y += d.y;
  }

  const AABB* tree_aabb = &nodes[proxy_id].aabb;
  if (aabb_contains(tree_aabb, aabb))
  {
    AABB huge_aabb = fat_aabb;
    aabb_extend(&huge_aabb, 4 * 5.f);

    if (aabb_contains(&huge_aabb, tree_aabb))
    {
      return false;
    }
  }

  aabb_tree_remove_leaf(AABBTree, proxy_id);

  nodes[proxy_id].aabb = fat_aabb;

  aabb_tree_insert_leaf(AABBTree, proxy_id);

  return true;
}

void aabb_tree_query(AABBTree* AABBTree, const AABB* aabb, Callback cb)
{
  Stack        stack;
  void(* fn)(void*, int); 
  TreeNode* nodes; 
  
  nodes = AABBTree->nodes;
  fn = (void(*)(void*, int)) cb.func;
  stack_init(&stack);
  stack_push(&stack, AABBTree->root);
  while (!stack_empty(&stack))
  {
    int id = stack_pop(&stack);
    if (id == NULL_NODE)
    {
      continue;
    }

    const TreeNode* node = nodes + id;
    if (aabb_test_overlap(aabb, &node->aabb))
    {
      if (is_leaf(node))
      {
        fn(cb.user_data, id);
      }
      else
      {
        stack_push(&stack, node->child1);
        stack_push(&stack, node->child2);
      }
    }
  }
  stack_finalize(&stack);
}

void* aabb_tree_get_user_data(AABBTree* AABBTree, int proxy_id)
{
  assert(proxy_id >= 0 && proxy_id < AABBTree->capacity);
  assert(is_leaf(&AABBTree->nodes[proxy_id]));
  return AABBTree->nodes[proxy_id].user_data;
}

const AABB* aabb_tree_get_fat_aabb(AABBTree* AABBTree, int proxy_id)
{
  assert(proxy_id >= 0 && proxy_id < AABBTree->capacity);
  assert(is_leaf(&AABBTree->nodes[proxy_id]));
  return &AABBTree->nodes[proxy_id].aabb;
}
