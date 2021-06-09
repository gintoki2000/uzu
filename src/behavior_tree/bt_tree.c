#include "behavior_tree/bt_tree.h"
#include "behavior_tree/bt_root.h"

typedef struct bt_BBKey
{
  char name[32];
} bt_BBKey;

typedef struct bt_BBKeySet
{
  u32       size;
  u32       count;
  bt_BBKey* keys;
} bt_BBKeySet;

typedef struct bt_Tree
{
  struct bt_Root* root;
  PointerArray*   nodes;
  bt_BBKeySet*    keySet;
} bt_Tree;

bt_BBKeySet* bt_bbkey_set_create(void)
{
  bt_BBKeySet* keySet = SDL_malloc(sizeof(bt_BBKeySet));

  keySet->count = 0;
  keySet->size  = 16;
  keySet->keys  = SDL_calloc(sizeof(bt_BBKeySet), keySet->size);

  return keySet;
}

void bt_bbkey_set_free(bt_BBKeySet* keySet)
{
  SDL_free(keySet->keys);
  SDL_free(keySet);
}

BOOL bt_bbkey_set_add(bt_BBKeySet* keySet, const char* key)
{
  if (bt_get_bbkey_index(keySet, key, NULL))
    return TRUE;

  if (keySet->count == keySet->size)
  {
    keySet->keys = SDL_realloc(keySet->keys, sizeof(bt_BBKey) * keySet->size * 2);
    keySet->size *= 2;
  }
  SDL_strlcpy(keySet->keys[keySet->count++].name,
              key,
              sizeof(keySet->keys[keySet->count - 1].name));
  return TRUE;
}

u32 bt_bbkey_set_count(bt_BBKeySet* keySet)
{
  return keySet->count;
}

BOOL bt_get_bbkey_index(bt_BBKeySet* keySet, const char* key, u32* outIndex)
{
  for (u32 i = 0; i < keySet->count; ++i)
    if (!SDL_strcmp(key, keySet->keys[i].name))
    {
      if (outIndex)
        *outIndex = i;
      return TRUE;
    }
  return FALSE;
}

static BOOL visit_callback(void* arg, bt_Node* node)
{
  bt_Tree* self = arg;
  node->tree    = self;
  node->id      = bt_tree_node_count(self);
  pointer_array_add(self->nodes, node);
  return TRUE;
}

bt_Tree* bt_tree_create(void)
{
  bt_Tree* self = SDL_malloc(sizeof(bt_Tree));
  self->root    = NULL;
  self->keySet  = NULL;
  self->nodes   = pointer_array_create((DestroyFunc)bt_node_free);
  return self;
}

void bt_tree_tick(bt_Tree* self, const bt_UpdateContext* ctx)
{
  bt_node_tick(self->root, ctx);
}

void bt_tree_set_root(bt_Tree* self, bt_Root* root)
{
  self->root = root;
  pointer_array_clear(self->nodes);
  bt_node_accept(self->root, self, visit_callback);
}

u32 bt_tree_node_count(bt_Tree* self)
{
  return self->nodes->count;
}

void bt_tree_set_key_set(bt_Tree* self, bt_BBKeySet* keySet)
{
  self->keySet = keySet;
}

bt_BBKeySet* bt_tree_key_set(bt_Tree* self)
{
  return self->keySet;
}
