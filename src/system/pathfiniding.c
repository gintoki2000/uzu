#include "constances.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"
#include "components.h"
#include "map.h"
#include "resources.h"
typedef struct GirdNode
{
  int   x;
  int   y;
  int   f;
} GirdNode;

//
#define NODE_HEAP_MAX_ITEMS 512
#define GIRD_MAX_COL MAP_MAX_COL
#define GIRD_MAX_ROW MAP_MAX_ROW

static GirdNode _nhItems[NODE_HEAP_MAX_ITEMS];
static int      _nhCount;

static void nh_clear(void);
static GirdNode nh_poll(void);

INLINE int cmpf(const GirdNode* l, const GirdNode* r)
{ return l->f - r->f; }

INLINE int  nh_left_child(int parent_index) 
{ return 2 * parent_index + 1; }

INLINE int  nh_right_child(int parent_index) 
{ return 2 * parent_index + 2; }

INLINE int  nh_parent(int child_index) 
{ return (child_index - 1) / 2; }

INLINE BOOL nh_has_left_child(int index)
{ return nh_left_child(index) < _nhCount; }

INLINE BOOL nh_has_right_child(int index)
{ return nh_right_child(index) < _nhCount; }

INLINE BOOL nh_has_parent(int index) 
{ return nh_parent(index) >= 0; }

static void nh_heapify_up(void)
{
  int index = _nhCount - 1;
  int parent_index;
  GirdNode tmp;

  while (nh_has_parent(index) 
      && cmpf(&_nhItems[nh_parent(index)], &_nhItems[index]) > 0)
  {
    parent_index = nh_parent(index);
    tmp = _nhItems[parent_index];
    _nhItems[parent_index] = _nhItems[index];
    _nhItems[index] = tmp;
    index = parent_index;
  }
}

static void nh_heapify_down(void)
{
  int      index = 0;
  GirdNode tmp;
  while (nh_has_left_child(index))
  {
    int smallerChildIndex = nh_left_child(index);
    if (nh_has_right_child(index) &&
        cmpf(&_nhItems[nh_right_child(index)], &_nhItems[nh_left_child(index)])< 0)
    {
      smallerChildIndex = nh_right_child(index);
    }

    if (cmpf(&_nhItems[index], &_nhItems[smallerChildIndex]) < 0)
      break;
    else
    {
      tmp = _nhItems[index];
      _nhItems[index] = _nhItems[smallerChildIndex];
      _nhItems[smallerChildIndex] = tmp;
      index = smallerChildIndex;
    }
  }
}

static void nh_clear(void)
{ _nhCount = 0; }

static GirdNode nh_poll(void)
{
  ASSERT(_nhCount > 0 && "heap empty");
  GirdNode item   = _nhItems[0];
  _nhItems[0] = _nhItems[_nhCount - 1];
  --_nhCount;
  nh_heapify_down();
  return item;
}

static void nh_insert(GirdNode node)
{
  ASSERT(_nhCount < NODE_HEAP_MAX_ITEMS);
  _nhItems[_nhCount++] = node;
  nh_heapify_up();
}

INLINE BOOL nh_empty(void)
{ return _nhCount == 0; }

//A*
static int h(int x1, int y1, int x2, int y2)
{ return SDL_sqrtf(SDL_abs(x1 - x2) + SDL_abs(y1 - y2)); }

static BOOL _visited[GIRD_MAX_ROW][GIRD_MAX_COL];
static int  _costSoFar[GIRD_MAX_ROW][GIRD_MAX_COL];
static int  _gird[GIRD_MAX_ROW][GIRD_MAX_COL];
static int  _columns;
static int  _rows;
static POINT _cameFrom[GIRD_MAX_ROW][GIRD_MAX_COL];

#define NUM_ADJ_NODES 4

static BOOL adjnode(int x, int y, int i, POINT* out)
{ 
  static int dx[NUM_ADJ_NODES] = { 1, -1, 0, 0 };
  static int dy[NUM_ADJ_NODES] = { 0, 0, 1, -1 };

  if (i >= NUM_ADJ_NODES)
    return FALSE;
  
  out->x = x + dx[i];
  out->y = y + dy[i];
  
  if (out->x < 0 || out->x >= _columns)
    return FALSE;
  if (out->y < 0 || out->y >= _rows)
    return FALSE;
  if (_gird[out->y][out->x])
    return FALSE;
  return TRUE;
}

static void construct_navgird(void)
{
  map_get_size(&_columns, &_rows);
 
  for (int i = 0; i < _rows; ++i)
    SDL_memset4(_gird[i], 0, _columns);

  for (int i = 0; i < _rows; ++i)
    for (int j = 0; j < _columns; ++j)
      if (!map_floor_at(j, i))
        _gird[i][j] = 1;

}

#define visit(x, y) _visited[y][x] = TRUE
#define point_equal(p1, p2) (((p1).x == (p2).x) &&((p1.y) == (p2.y)))

static BOOL find_path(POINT start, POINT goal, POINT* outPath, int* outCount)
{
  GirdNode current, node;
  POINT next;
  int newCost;
  BOOL foundPath;

  if (_gird[start.y][start.x])
    return FALSE;
  if (_gird[goal.y][goal.x])
    return FALSE;
  if (point_equal(start, goal))
    return FALSE;

  for (int i = 0; i < _rows; ++i)
    SDL_memset(_visited, 0, sizeof(BOOL) * _columns);

  int maxValueOfnterger = INT32_MAX;
  Uint32 v  = *SDL_reinterpret_cast(Uint32*, &maxValueOfnterger);
  for (int i = 0; i < _rows; ++i)
    SDL_memset4(_costSoFar[i], v, _columns);

  nh_clear();

  _costSoFar[start.y][start.x] = 0;
  node.x = start.x;
  node.y = start.y;
  node.f = _costSoFar[start.y][start.x] + h(start.x, start.y, goal.x, goal.y);
  visit(start.x, start.y);

  nh_insert(node);

  foundPath = FALSE;

  while(!nh_empty())
  {
    current = nh_poll(); 
    if (current.x == goal.x && current.y == goal.y)
    {
      foundPath = TRUE;
      break;
    }

    for (int i = 0; adjnode(current.x, current.y, i, &next); ++i)
    {
      newCost = _costSoFar[current.y][current.x] + 1;
      if (!_visited[next.y][next.x] || newCost < _costSoFar[next.y][next.x])
      {
         _costSoFar[next.y][next.x] = newCost;
        nh_insert((GirdNode){
          .x = next.x,
          .y = next.y,
          .f =  h(next.x, next.y, goal.x, goal.y),
        });
        _cameFrom[next.y][next.x] = (POINT){ current.x, current.y };
        visit(next.x, next.y);
      }
    }
  }
  if (foundPath) 
  {
    int   count = 0;
    POINT p = goal;
    outPath[count++] = p;
    while(!point_equal(_cameFrom[p.y][p.x], start))
    {
        outPath[count++] = _cameFrom[p.y][p.x];
        p = _cameFrom[p.y][p.x];
    }
    outPath[count++] = start;
    *outCount = count;
    return TRUE;
  }
  return FALSE;
}

static void on_level_loaded(SDL_UNUSED void* data, SDL_UNUSED const LevelLoadedMsg* msg)
{
  construct_navgird(); 
}

void pathfinding_system_init(void)
{
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(on_level_loaded)); 
}

static void rcpy(POINT* in, POINT* out, int count)
{
  int i,j;
  for (i = count - 1, j = 0; i >= 0; --i, ++j)
    out[j] = in[i];
}

extern ecs_Registry* gRegistry;
static POINT _nodeBuff[100];
void pathfinding_system(void)
{
   ecs_entity_t* entities;
   ecs_size_t count;
   PathfindingParams* params;
   Path* path;
   int nodeCount;
   void (*completedCallback)(void*, ecs_Registry*, ecs_entity_t, Path*);
   ecs_raw (gRegistry, PATHFINDING_PARAMS, &entities, (void**)&params, &count);
   for (int i = count -1; i >= 0; --i)
   {
     completedCallback = SDL_static_cast(void(*)(void*, ecs_Registry*, ecs_entity_t, Path*), params[i].cbCompleted.func);
     if (find_path(params[i].start, params[i].goal, _nodeBuff, &nodeCount)){
        path = ecs_add (gRegistry, entities[i], PATH);
        path->count        = nodeCount;
        path->currentIndex = 0;
        rcpy(_nodeBuff, path->nodes, nodeCount);
        if (completedCallback)
          completedCallback(params[i].cbCompleted.userData, gRegistry, entities[i], path);
     } else {
        if (completedCallback)
          completedCallback(params[i].cbCompleted.userData, gRegistry, entities[i], NULL);
     } 
     ecs_rmv(gRegistry, entities[i], PATHFINDING_PARAMS);
   }
}

#define MAX_TILES ((WIN_WIDTH * WIN_HEIGHT) / TILE_SIZE)

static RECT _rectBuff[MAX_TILES];
void draw_gird(void)
{
  extern RENDERER* gRenderer;
  extern RECT gViewport;
  RECT r; 

  s32 startX, startY, endX, endY;
  int c = 0;

  startX = gViewport.x / TILE_SIZE;
  startY = gViewport.y / TILE_SIZE;

  endX = (gViewport.x + gViewport.w) / TILE_SIZE;
  endY = (gViewport.y + gViewport.h) / TILE_SIZE;

  startX = max(0, startX);
  startY = max(0, startY);

  endX = min(endX, _columns - 1);
  endY = min(endY, _rows - 1);
  r.w = 4;
  r.h = 4;
  FC_Font* font = get_font(FONT_ITEM_PICKED_UP);
  for (int y = startY; y <= endY; ++y)
    for (int x = startX; x <= endX; ++x)
    {
      if (_gird[y][x]){
        r.x = x * TILE_SIZE - gViewport.x + 6;
        r.y = y * TILE_SIZE - gViewport.y + 6;
        if (c >= MAX_TILES)
          printf("het cho trong buffer\n");
        else 
          _rectBuff[c++] = r; 
      } 

      if (_costSoFar[y][x] == INT32_MAX || _costSoFar[y][x] == 0)
        continue;
      r.x = x * TILE_SIZE - gViewport.x + 6;
      r.y = y * TILE_SIZE - gViewport.y + 6;
      FC_Draw(font, gRenderer, r.x, r.y, "%d", _costSoFar[y][x]);
    }
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0x00, 0x00, 0xd0);
  SDL_RenderFillRects(gRenderer, _rectBuff, c);

  int tileX, tileY, mouseX, mouseY;
  SDL_GetMouseState(&mouseX, &mouseY);
  mouseX = mouseX / SCL_X;
  mouseY = mouseY / SCL_Y;
  tileX = mouseX + gViewport.x;
  tileY = mouseY + gViewport.y;
  tileX /= TILE_SIZE;
  tileY /= TILE_SIZE;
  FC_DrawColor(get_font(FONT_ITEM_PICKED_UP), gRenderer, mouseX, mouseY,
      _gird[tileY][tileX] ?  (COLOR){0xff, 0x00, 0x00, 0xff} : (COLOR){ 0xff, 0xff, 0xff, 0xff } ,"(%d, %d)", tileX, tileY);
}



