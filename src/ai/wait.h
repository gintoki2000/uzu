#ifndef WAIT_H
#define WAIT_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(BTNode)
  int total_ticks;
  int remaining;
} BTTask_Wait;

const BTNodeVtbl* wait_vtbl_inst();
void               wait_vtbl_init(BTNodeVtbl* vtbl);

BTTask_Wait*     wait_new(int total_ticks);
BTTask_Wait*     wait_init(BTTask_Wait* self, int total_ticks);
void      wait_abort(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity);
void      wait_finish(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);
BTStatus wait_exec(BTTask_Wait* self, Ecs* ecs, ecs_entity_t entity);
#endif // WAIT_H
