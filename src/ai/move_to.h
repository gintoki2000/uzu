#ifndef MOVE_TO_H
#define MOVE_TO_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(bt_Node)
  float slow_down_radius;
  float arrive_radius;
} MoveTo;

MoveTo* move_to_new(float slow_down_radius, float arrive_radius);
#endif // MOVE_TO_H
