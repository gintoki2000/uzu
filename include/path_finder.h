#ifndef PATH_FINDER_H
#define PATH_FINDER_H
#include "toolbox/toolbox.h"

typedef struct
{
  Vec2i start;
  Vec2i end;
} PathFindingInput;

typedef struct PathFindingOutput
{
  int    num_nodes;
  int    max_dist;
  Vec2i* nodes;
} PathFindingOutput;

BOOL find_path(int src_x, int src_y, int dst_x, int dst_y, Vec2i* path, int* cnt);

#endif // PATH_FINDER_H
