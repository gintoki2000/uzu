#ifndef BTT_WAIT_H
#define BTT_WAIT_H
#include "behaviour_tree.h"
typedef struct btt_Wait
{
  __EXTEND_BT_NODE__
  int ticks;
  int initialTicks;
} btt_Wait;

btt_Wait* btt_wait_new(int ticks);
btt_Wait* btt_wait_init(btt_Wait* self, int ticks);
void      btt_wait_start(btt_Wait* self);
void      btt_wait_exec(btt_Wait* self, Ecs* ecs, ecs_entity_t entity);
#endif // BTT_WAIT_H
