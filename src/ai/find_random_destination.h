#ifndef FIND_RANDOM_LOCATION_H
#define FIND_RANDOM_LOCATION_H
#include <behaviour_tree.h>
typedef struct
{
  BT_EXTEND_NODE(BTNode) 
} FindRandomDestination;

FindRandomDestination* find_random_destination_new();
#endif //FIND_RANDOM_LOCATION_H
