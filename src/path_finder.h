#ifndef PATH_FINDER_H
#define PATH_FINDER_H
#include <toolbox/toolbox.h>

typedef struct pf_Node
{
  int x, y;
} pf_Node;

BOOL find_path(int src_x, int src_y, int dst_x, int dst_y, pf_Node* path, int* cnt);

#endif // PATH_FINDER_H
