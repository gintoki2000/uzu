
#include "path_finder.h"
#include "map.h"
#include <toolbox/toolbox.h>

typedef struct pf_GirdNode
{
  int   x, y;
  float priority;
} pf_GirdNode;

/*
INLINE float distance(int x1, int y1, int x2, int y2)
{
  float a = x1 - x2;
  float b = y1 - y2;
  return SDL_sqrtf(a * a + b * b);
}*/

INLINE float heuristic(int x1, int y1, int x2, int y2) { return ABS(x1 - x2) + ABS(y1 - y2); }

static int compare_node_priority(const pf_GirdNode* a, const pf_GirdNode* b)
{
  return a->priority - b->priority;
}

static pf_GirdNode* grid_node_new(int x, int y, float priority)
{
  pf_GirdNode* p = malloc(sizeof(pf_GirdNode));
  p->x           = x;
  p->y           = y;
  p->priority    = priority;
  return p;
}

static void adj(int x, int y, int* neighbor_x, int* neighbor_y, int* cnt)
{

  static const int DX[4] = { 0, 0, -1, 1 };
  static const int DY[4] = { -1, 1, 0, 0 };

  int w, h;
  int nx;
  int ny;

  map_get_size(&w, &h);
  *cnt = 0;
  for (int i = 0; i < 4; ++i)
  {
    nx = x + DX[i];
    ny = y + DY[i];
    if (nx < 0 || nx >= w)
      continue;
    if (ny < 0 || ny >= h)
      continue;
    if (map_is_floor(nx, ny))
    {
      neighbor_x[*cnt] = nx;
      neighbor_y[*cnt] = ny;
      (*cnt)++;
    }
  }
}

static void retrieve(const pf_GirdNode* came_from, int dst_x, int dst_y, pf_Node* path, int* cnt)
{
  int x, y, w, h, i;

  x = dst_x;
  y = dst_y;
  map_get_size(&w, &h);
  *cnt = 0;

  while (x != -1 && y != -1)
  {
    ASSERT(*cnt < 100 - 1);
    path[(*cnt)++] = (pf_Node){ x, y };

    i = y * w + x;
    x = came_from[i].x;
    y = came_from[i].y;
  }
  for (int i = 0; i < (*cnt) / 2; ++i)
    SWAP(path[i], path[(*cnt) - i - 1]);
}

BOOL find_path(int x1, int y1, int x2, int y2, pf_Node* path, int* cnt)
{
  BOOL*        visited   = NULL;
  pf_GirdNode* came_from = NULL;
  Heap*        queue     = NULL; /* Heap<pf_GirdNode> */
  pf_GirdNode* curr;
  int          curr_x, curr_y, index;
  int          next_x[4] = { 0 }, next_y[4] = { 0 };
  int          neighbor_cnt, tile_cnt;
  float        priority;
  BOOL         found = false;
  int          w, h;

  if (x1 == x2 && y1 == y2)
    return FALSE;

  map_get_size(&w, &h);
  tile_cnt  = w * h;
  visited   = calloc(tile_cnt, sizeof(BOOL));
  came_from = calloc(tile_cnt, sizeof(pf_GirdNode));
  queue     = heap_new((compare_fn_t)compare_node_priority, (destroy_fn_t)free);

  for (int i = 0; i < tile_cnt; ++i)
    came_from[i] = (pf_GirdNode){ -1, -1, -1.f };

  visited[y1 * w + x1] = true;

  heap_add(queue, grid_node_new(x1, y1, 0));

  while (HEAP(queue)->cnt > 0)
  {
    curr   = heap_poll(queue);
    curr_x = curr->x;
    curr_y = curr->y;

    if (curr_x == x2 && curr_y == y2)
    {
      found = true;
      break;
    }

    adj(curr_x, curr_y, next_x, next_y, &neighbor_cnt);

    for (int i = 0; i < neighbor_cnt; ++i)
    {
      index = next_y[i] * w + next_x[i];
      if (!visited[index])
      {
        visited[index]   = true;
        came_from[index] = (pf_GirdNode){ curr_x, curr_y, -1.f };
        priority         = heuristic(x2, y2, next_x[i], next_y[i]);
        heap_add(queue, grid_node_new(next_x[i], next_y[i], priority));
      }
    }
    free(curr);
  }

  if (found)
  {
    retrieve(came_from, x2, y2, path, cnt);
  }

  free(visited);
  free(came_from);
  heap_delete(queue);
  return found;
}
