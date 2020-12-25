#include "rtree.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define RTREE_DEFAULT_CAP 512

typedef struct
{
  AABB  aabb;
  void* user_data;
  int   parent;
  int   child1;
  int   child2;
  int   height;
} TreeNode;

struct RTree
{
  int       capacity;
  int       count;
  TreeNode* nodes;
  int       root;
  int       free_index;
  int*      next;
};

static RTree* rtree_init(RTree* self)
{
  self->capacity = RTREE_DEFAULT_CAP;
  self->count    = 0;
  self->root     = NULL_NODE;
  self->nodes    = calloc(RTREE_DEFAULT_CAP, sizeof(TreeNode));
  self->next     = calloc(RTREE_DEFAULT_CAP, sizeof(int));
  for (int i = 0; i < RTREE_DEFAULT_CAP - 1; ++i)
  {
    self->next[i]         = i + 1;
    self->nodes[i].height = -1;
  }
  self->free_index                  = 0;
  self->next[RTREE_DEFAULT_CAP - 1] = NULL_NODE;
  return self;
}

static bool node_is_leaf(const TreeNode* node) { return node->child1 == -1; }

static void rtree_finalize(RTree* self)
{
  free(self->nodes);
  free(self->next);
}
static void rtree_grow_if_need(RTree* self)
{
  if (self->count == self->capacity)
  {
    ASSERT(self->free_index == NULL_NODE);

    self->capacity *= 2;
    self->nodes = realloc(self->nodes, self->capacity * sizeof(TreeNode));
    self->next  = realloc(self->next, self->capacity * sizeof(int));

    for (int i = self->count; i < self->capacity - 1; ++i)
    {
      self->next[i]         = i + 1;
      self->nodes[i].height = -1;
    }
    self->next[self->capacity - 1]         = NULL_NODE;
    self->nodes[self->capacity - 1].height = -1;
    self->free_index                       = self->count;
  }
}

static int rtree_allocate_node(RTree* self)
{

  ASSERT(self->free_index != -1);

  TreeNode* nodes      = self->nodes;
  int       idx        = self->free_index;
  self->free_index     = self->next[idx];
  nodes[idx].user_data = NULL;
  nodes[idx].parent    = NULL_NODE;
  nodes[idx].child1    = NULL_NODE;
  nodes[idx].child2    = NULL_NODE;
  nodes[idx].height    = 0;
  ++self->count;
  return idx;
}

static void rtree_free_node(RTree* self, int node_idx)
{
  ASSERT(node_idx >= 0 && node_idx < self->capacity);
  ASSERT(self->count > 0);

  self->next[node_idx]         = self->free_index;
  self->free_index             = node_idx;
  self->nodes[node_idx].height = -1;
  --self->count;
}

static int rtree_balance(RTree* self, int idx)
{
  int ia = idx;
  ASSERT(ia != NULL_NODE);
  TreeNode* nodes = self->nodes;
  TreeNode* a     = &nodes[ia];
  if (node_is_leaf(a) || a->height < 2)
  {
    return ia;
  }

  int       ib      = a->child1;
  int       ic      = a->child2;
  TreeNode* b       = nodes + ib;
  TreeNode* c       = nodes + ic;
  int       balance = c->height - b->height;
  if (balance > 1)
  {
    int       ih = c->child1;
    int       ik = c->child2;
    TreeNode* h  = nodes + ih;
    TreeNode* k  = nodes + ik;

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
      self->root = ic;
    }

    if (h->height > k->height)
    {
      c->child2 = ih;
      a->child2 = ik;
      k->parent = ia;
      a->aabb   = aabb_merge(&b->aabb, &k->aabb);
      c->aabb   = aabb_merge(&a->aabb, &h->aabb);
      a->height = 1 + max(b->height, k->height);
      c->height = 1 + max(a->height, h->height);
    }
    else
    {
      c->child2 = ik;
      a->child2 = ih;
      h->parent = ia;
      a->aabb   = aabb_merge(&b->aabb, &h->aabb);
      c->aabb   = aabb_merge(&a->aabb, &k->aabb);
      a->height = 1 + max(b->height, h->height);
      c->height = 1 + max(a->height, k->height);
    }

    return ic;
  }

  if (balance < -1)
  {
    int       id = b->child1;
    int       ie = b->child2;
    TreeNode* d  = nodes + id;
    TreeNode* e  = nodes + ie;
    ASSERT(0 <= id && id < self->capacity);
    ASSERT(0 <= ie && ie < self->capacity);

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
        ASSERT(nodes[b->parent].child2 == ia);
        nodes[b->parent].child2 = ib;
      }
    }
    else
    {
      self->root = ib;
    }

    if (d->height > e->height)
    {
      b->child2 = id;
      a->child1 = ie;
      e->parent = ia;
      a->aabb   = aabb_merge(&c->aabb, &e->aabb);
      b->aabb   = aabb_merge(&a->aabb, &d->aabb);

      a->height = 1 + max(c->height, e->height);
      b->height = 1 + max(a->height, d->height);
    }
    else
    {
      b->child2 = ie;
      a->child1 = id;
      d->parent = ia;
      a->aabb   = aabb_merge(&c->aabb, &d->aabb);
      b->aabb   = aabb_merge(&a->aabb, &e->aabb);

      a->height = 1 + max(c->height, d->height);
      b->height = 1 + max(a->height, e->height);
    }

    return ib;
  }

  return ia;
}

static void rtree_insert_leaf(RTree* self, int leaf)
{
  TreeNode* nodes = self->nodes;
  if (self->root == NULL_NODE)
  {
    self->root               = leaf;
    nodes[self->root].parent = NULL_NODE;
    return;
  }

  AABB leaf_aabb = nodes[leaf].aabb;
  int  index     = self->root;
  while (node_is_leaf(&nodes[index]) == false)
  {
    int child1 = nodes[index].child1;
    int child2 = nodes[index].child2;

    float area = aabb_premiter(&nodes[index].aabb);

    AABB  combined_aabb = aabb_merge(&nodes[index].aabb, &leaf_aabb);
    float combine_area  = aabb_premiter(&combined_aabb);

    float cost = 2.0f * combine_area;

    float inheritance_cost = 2.0f * (combine_area - area);

    float cost1;
    if (node_is_leaf(&nodes[child1]))
    {
      AABB aabb = aabb_merge(&leaf_aabb, &nodes[child1].aabb);
      cost1     = aabb_premiter(&aabb) + inheritance_cost;
    }
    else
    {
      AABB  aabb    = aabb_merge(&leaf_aabb, &nodes[child1].aabb);
      float oldArea = aabb_premiter(&nodes[child1].aabb);
      float newArea = aabb_premiter(&aabb);
      cost1         = (newArea - oldArea) + inheritance_cost;
    }

    float cost2;
    if (node_is_leaf(&nodes[child2]))
    {
      AABB aabb = aabb_merge(&leaf_aabb, &nodes[child2].aabb);
      cost2     = aabb_premiter(&aabb) + inheritance_cost;
    }
    else
    {
      AABB  aabb    = aabb_merge(&leaf_aabb, &nodes[child2].aabb);
      float oldArea = aabb_premiter(&nodes[child2].aabb);
      float newArea = aabb_premiter(&aabb);
      cost2         = newArea - oldArea + inheritance_cost;
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
  int old_parent              = nodes[sibling].parent;
  int new_parent              = rtree_allocate_node(self);
  nodes[new_parent].parent    = old_parent;
  nodes[new_parent].user_data = NULL;
  nodes[new_parent].aabb      = aabb_merge(&leaf_aabb, &nodes[sibling].aabb);
  nodes[new_parent].height    = nodes[sibling].height + 1;

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
    nodes[sibling].parent    = new_parent;
    nodes[leaf].parent       = new_parent;
  }
  else
  {
    // The sibling was the root.
    nodes[new_parent].child1 = sibling;
    nodes[new_parent].child2 = leaf;
    nodes[sibling].parent    = new_parent;
    nodes[leaf].parent       = new_parent;
    self->root               = new_parent;
  }

  // Walk back up the tree fixing heights and AABBs
  index = nodes[leaf].parent;
  while (index != NULL_NODE)
  {
    index = rtree_balance(self, index);

    int child1 = nodes[index].child1;
    int child2 = nodes[index].child2;

    ASSERT(child1 != NULL_NODE);
    ASSERT(child2 != NULL_NODE);

    nodes[index].height = 1 + max(nodes[child1].height, nodes[child2].height);
    nodes[index].aabb   = aabb_merge(&nodes[child1].aabb, &nodes[child2].aabb);

    index = nodes[index].parent;
  }
}

static void rtree_remove_leaf(RTree* self, int leaf)
{

  TreeNode* nodes = self->nodes;
  if (leaf == self->root)
  {
    self->root = NULL_NODE;
    return;
  }

  int parent       = nodes[leaf].parent;
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
    rtree_free_node(self, parent);

    int index = grand_parent;
    while (index != NULL_NODE)
    {
      index = rtree_balance(self, index);

      int child1 = nodes[index].child1;
      int child2 = nodes[index].child2;

      nodes[index].aabb   = aabb_merge(&nodes[child1].aabb, &nodes[child2].aabb);
      nodes[index].height = 1 + max(nodes[child1].height, nodes[child2].height);

      index = nodes[index].parent;
    }
  }
  else
  {
    self->root            = sibling;
    nodes[sibling].parent = NULL_NODE;
    rtree_free_node(self, parent);
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
  int n       = 64;
  s->capacity = n;
  s->count    = 0;
  s->items    = calloc(n, sizeof(int));
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
  ASSERT(s->count > 0 && "stack empty");
  return s->items[--s->count];
}
static bool stack_empty(Stack* s) { return s->count == 0; }

/********************************************************************/

RTree* rtree_new() { return rtree_init(malloc(sizeof(RTree))); }

void rtree_delete(RTree* self)
{
  if (self)
  {
    rtree_finalize(self);
    free(self);
  }
}

int rtree_create_proxy(RTree* self, void* user_data, const AABB* aabb)
{
  int       idx   = rtree_allocate_node(self);
  TreeNode* nodes = self->nodes;

  nodes[idx].aabb      = *aabb;
  nodes[idx].parent    = NULL_NODE;
  nodes[idx].child1    = NULL_NODE;
  nodes[idx].child2    = NULL_NODE;
  nodes[idx].height    = 0;
  nodes[idx].user_data = user_data;

  aabb_extend(&nodes[idx].aabb, 5.f);

  rtree_insert_leaf(self, idx);

  return idx;
}

void rtree_destroy_proxy(RTree* self, int proxy_id)
{
  ASSERT(0 <= proxy_id && proxy_id < self->capacity);
  ASSERT(node_is_leaf(&self->nodes[proxy_id]));

  rtree_remove_leaf(self, proxy_id);
  rtree_free_node(self, proxy_id);
}

bool rtree_move_proxy(RTree* self, int proxy_id, const AABB* aabb, const Vec2 displacement)
{
  TreeNode* nodes = self->nodes;
  ASSERT(0 <= proxy_id && proxy_id < self->capacity);

  ASSERT(node_is_leaf(&nodes[proxy_id]));

  AABB fat_aabb = *aabb;
  aabb_extend(&fat_aabb, 5.f);

  Vec2 d = vec2_mul(displacement, 2.f);

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

  rtree_remove_leaf(self, proxy_id);

  nodes[proxy_id].aabb = fat_aabb;

  rtree_insert_leaf(self, proxy_id);

  return true;
}

void rtree_query(RTree* self, const AABB* aabb, Callback cb)
{
  Stack stack;
  void (*fn)(void*, int);
  TreeNode* nodes;

  nodes = self->nodes;
  fn    = (void (*)(void*, int))cb.func;
  stack_init(&stack);
  stack_push(&stack, self->root);
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
      if (node_is_leaf(node))
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

void* rtree_get_user_data(RTree* self, int proxy_id)
{
  ASSERT(proxy_id >= 0 && proxy_id < self->capacity);
  ASSERT(node_is_leaf(&self->nodes[proxy_id]));
  return self->nodes[proxy_id].user_data;
}

const AABB* rtree_get_fat_aabb(RTree* self, int proxy_id)
{
  ASSERT(proxy_id >= 0 && proxy_id < self->capacity);
  ASSERT(node_is_leaf(&self->nodes[proxy_id]));
  return &self->nodes[proxy_id].aabb;
}

void rtree_draw(RTree* self, SDL_Renderer* renderer, const SDL_Rect* viewport)
{
  Stack     stack;
  SDL_Rect  rect;
  TreeNode* node;
  if (self->root == NULL_NODE)
    return;
  stack_init(&stack);

  SDL_SetRenderDrawColor(renderer, 224, 74, 214, 255);
  stack_push(&stack, self->root);
  while (!stack_empty(&stack))
  {
    node   = &self->nodes[stack_pop(&stack)];
    rect.x = node->aabb.lower_bound.x - viewport->x;
    rect.y = node->aabb.lower_bound.y - viewport->y;

    rect.w = node->aabb.upper_bound.x - node->aabb.lower_bound.x;
    rect.h = node->aabb.upper_bound.y - node->aabb.lower_bound.y;
    SDL_RenderDrawRect(renderer, &rect);

    if (!node_is_leaf(node))
    {
      stack_push(&stack, node->child1);
      stack_push(&stack, node->child2);
    }
  }
  stack_finalize(&stack);
}
