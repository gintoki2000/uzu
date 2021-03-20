
#include "path_finder.h"
#include "map.h"
#include "toolbox/toolbox.h"

static int _fcost[MAP_MAX_ROW * MAP_MAX_COL];
static int _gcost[MAP_MAX_ROW * MAP_MAX_COL];
static int _hcost[MAP_MAX_ROW * MAP_MAX_COL];
static int _is_walkable[MAP_MAX_ROW * MAP_MAX_COL];
static int _camefrom[MAP_MAX_ROW * MAP_MAX_COL];
static Vec2i _location[MAP_MAX_ROW * MAP_MAX_COL];
static BOOL _visit[MAP_MAX_ROW * MAP_MAX_COL];
static int _index[MAP_MAX_ROW * MAP_MAX_COL];

Heap _priority_queue;

static int compute_index(int x, int y, int gird_width)
{ return x + y * gird_width; }

static int compare_fcost(const int* a, const int* b)
{  return _fcost[*a] - _fcost[*b]; }

static int heuristic(Vec2i a, Vec2i b)
{ return abs(a.x - b.x) + abs(a.y - b.y); }

void path_finder_init()
{
  heap_init(&_priority_queue, (compare_fn_t)compare_fcost, NULL);
}

void path_finder_fini()
{
  heap_fini(&_priority_queue);
}

void find_path(Vec2i start_location, Vec2i end_location)
{
  int gird_width, gird_height, total_nodes;
  map_get_size(&gird_width, &gird_height);
  total_nodes = gird_height * gird_height;
  int start_index = compute_index(start_location.x, start_location.y, gird_width);
  int end_index = compute_index(end_location.x, end_location.y, gird_width);
  int current_index;

  for (int i = 0; i < total_nodes; ++i)
  {
    _camefrom[i] = -1;
    _location[i] = (Vec2i){ i % gird_width, i / gird_width };
    _hcost[i] = heuristic(_location[i], end_location);
    _gcost[i] = INT32_MAX;
    _index[i] = i;
  }

  _gcost[start_index] = 0;
  _visit[start_index] = TRUE;
  heap_add(&_priority_queue, &_index[start_index]);

  while (!heap_empty(&_priority_queue))
  {
      current_index = *(int*)heap_poll(&_priority_queue); 
      if (current_index == end_index)
        break;

  }
}
