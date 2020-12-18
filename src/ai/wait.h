#ifndef WAIT_H
#define WAIT_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(bt_Node)
  int total_ticks;
  int remaining;
} Wait;

const bt_NodeVtbl* wait_vtbl_inst();
void               wait_vtbl_init(bt_NodeVtbl* vtbl);

Wait*     wait_new(int total_ticks);
Wait*     wait_init(Wait* self, int total_ticks);
void      wait_abort(Wait* self, Ecs* ecs, ecs_entity_t entity);
void      wait_finish(Wait* self, Ecs* ecs, ecs_entity_t entity, bt_Status finish_status);
bt_Status wait_exec(Wait* self, Ecs* ecs, ecs_entity_t entity);
#endif // WAIT_H
