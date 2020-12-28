#ifndef MOVE_TO_H
#define MOVE_TO_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(BTNode)
  float arrive_radius;
} MoveTo;

MoveTo* move_to_new(float arrive_radius);
#endif // MOVE_TO_H
