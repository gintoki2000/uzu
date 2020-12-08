#ifndef BBT_MOVE_TO_H
#define BBT_MOVE_TO_H
#include "behaviour_tree.h"

typedef struct btt_MoveTo
{
  __EXTEND_BT_NODE__
} btt_MoveTo;

btt_MoveTo* btt_move_to_new();
void        btt_move_to_fini(btt_MoveTo* self);
void        btt_move_to_start(btt_MoveTo* self);
void        btt_move_to_exec(btt_MoveTo* self, Ecs* ecs, ecs_entity_t entity);
#endif // BBT_MOVE_TO_H
