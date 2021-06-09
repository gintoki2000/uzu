// ngotrung Sun 06 Jun 2021 12:58:31 PM +07
#ifndef BT_GET_RANDOM_LOCATION_TASK_H
#define BT_GET_RANDOM_LOCATION_TASK_H
#include "toolbox/common.h"
typedef struct bt_Node bt_Node;

bt_Node* bt_get_random_location_task_create(u32 resultKey, float radius);
#endif // BT_GET_RANDOM_LOCATION_TASK_H
