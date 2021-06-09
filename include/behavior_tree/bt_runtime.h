// ngotrung Sun 30 May 2021 09:15:50 AM +07
#ifndef BT_RUNTIME_H
#define BT_RUNTIME_H
#include "toolbox/toolbox.h"
#define BT_MAX_RUNTIME_DATA_SIZE 64
#define BT_RUNTIME_DATA_DECL(...)                                                                  \
  typedef struct RuntimeData __VA_ARGS__ RuntimeData;                                              \
  STATIC_ASSERT(sizeof(RuntimeData) <= BT_MAX_RUNTIME_DATA_SIZE,                                   \
                runtime_data_size_must_be_smaller_max_size);
typedef struct bt_RuntimeDataManager bt_RuntimeDataManager;

struct bt_Node;
bt_RuntimeDataManager* bt_runtime_data_manager_create(u32 total);
void               bt_runtime_data_manager_free(bt_RuntimeDataManager* self);
void*              bt_get_runtime_data(bt_RuntimeDataManager* mgr, struct bt_Node* node);
#endif // BT_RUNTIME_H
