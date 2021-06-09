#include "behavior_tree/bt_runtime.h"
#include "behavior_tree/bt_node.h"
typedef struct bt_RuntimeData
{
  u8 raw[BT_MAX_RUNTIME_DATA_SIZE];
} bt_RuntimeData;

struct bt_RuntimeDataManager
{
  u32             total;
  bt_RuntimeData* data;
};

bt_RuntimeDataManager* bt_runtime_data_manager_create(u32 total)
{
  bt_RuntimeDataManager* self = SDL_malloc(sizeof(bt_RuntimeDataManager));
  self->total             = total;
  self->data              = SDL_malloc(total * sizeof(bt_RuntimeData));
  return self;
}

void bt_runtime_data_manager_free(bt_RuntimeDataManager* self)
{
  if (self)
  {
    SDL_free(self->data);
    SDL_free(self);
  }
}

void* bt_get_runtime_data(bt_RuntimeDataManager* self, bt_Node* node)
{
  u32 nodeID = bt_node_get_id(node);
  ASSERT_MSG(nodeID < self->total, "Invalid ID");
  return &(self->data[nodeID]);
}
