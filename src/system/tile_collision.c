#include "components.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include <constances.h>
#include <map.h>
#include <toolbox/toolbox.h>
#include "config.h"

struct _Components
{
  Transform* transform;
  HitBox*    hitbox;
};

extern Ecs*      gEcs;
extern RECT      gViewport;
extern RENDERER* gRenderer;

#define NUM_TILES 65

static BOOL check_midle(int, int, int, int);
static BOOL check_right(int, int, int, int);
static BOOL check_left(int, int, int, int);

#if DEBUG
/*****************************************************/
/* debug functions                                   */
/*****************************************************/
static void draw_midle(int, int);
static void draw_left(int, int);
static void draw_right(int, int);
#endif

#define _(id, fn) [id] = fn
static BOOL (*const _checkFuncTbl[NUM_TILES])(int, int, int, int) = {
  _(14, check_midle), _(15, check_midle), _(16, check_midle), _(17, check_midle),
  _(19, check_right), _(20, check_left),  _(21, check_midle), _(22, check_midle),
  _(23, check_midle), _(24, check_midle), _(34, check_midle), _(35, check_midle),
  _(36, check_midle), _(37, check_midle), _(38, check_midle), _(39, check_midle),
  _(41, check_midle), _(43, check_midle), _(44, check_midle), _(45, check_right),
  _(46, check_left),  _(47, check_midle), _(48, check_midle), _(51, check_midle),
  _(52, check_midle), _(53, check_midle), _(54, check_left),  _(55, check_right),
  _(56, check_left),  _(57, check_right),
};

#if DEBUG
static void (*const _debugFuncTbl[NUM_TILES])(int, int) = {
  _(14, draw_midle), _(15, draw_midle), _(16, draw_midle), _(17, draw_midle), _(19, draw_right),
  _(20, draw_left),  _(21, draw_midle), _(22, draw_midle), _(23, draw_midle), _(24, draw_midle),
  _(34, draw_midle), _(35, draw_midle), _(36, draw_midle), _(37, draw_midle), _(38, draw_midle),
  _(39, draw_midle), _(41, draw_midle), _(43, draw_midle), _(44, draw_midle), _(45, draw_right),
  _(46, draw_left),  _(47, draw_midle), _(48, draw_midle), _(51, draw_midle), _(52, draw_midle),
  _(53, draw_midle), _(54, draw_left),  _(55, draw_right), _(56, draw_left),  _(57, draw_right),
};
#endif
#undef _

static const ecs_size_t _componentIds[] = { TRANSFORM, HITBOX };

INLINE BOOL has_collider(tile_t tile)
{
  return tile && _checkFuncTbl[tile - 1];
}

INLINE BOOL check(int obj_x, int obj_y, int cell_x, int cell_y)
{
  tile_t tile = map_at(MAP_LAYER_WALL, cell_x, cell_y);
  if (tile > 0 && _checkFuncTbl[tile - 1] != NULL)
  {
    return _checkFuncTbl[tile - 1](obj_x, obj_y, cell_x, cell_y);
  }
  return FALSE;
}


#if DEBUG
void draw_map_colliders(void)
{
  int w, h;
  int start_x, start_y, end_x, end_y;

  map_get_size(&w, &h);

  start_x = gViewport.x / TILE_SIZE;
  start_y = gViewport.y / TILE_SIZE;

  end_x = (gViewport.x + gViewport.w) / TILE_SIZE;
  end_y = (gViewport.y + gViewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, w - 1);
  end_y = min(end_y, h - 1);

  tile_t tile;

  for (int y = start_y; y <= end_y; ++y)
    for (int x = start_x; x <= end_x; ++x)
    {
      tile = map_at(MAP_LAYER_WALL, x, y);
      if (tile > 0 && _debugFuncTbl[tile - 1] != NULL)
        _debugFuncTbl[tile - 1](x, y);
    }
}
#endif

INLINE int get_obj_bottom(const struct _Components c)
{
  return c.transform->position.y - c.hitbox->anchor.y + c.hitbox->size.y;
}

INLINE int get_obj_left(const struct _Components c)
{
  return c.transform->position.x - c.hitbox->anchor.x;
}

INLINE int get_obj_right(const struct _Components c)
{
  return get_obj_left(c) + c.hitbox->size.x;
}

void tile_collision_system()
{
  ecs_entity_t*      entities;
  ecs_size_t         cnt;
  struct _Components components;

  int  objBottom, objLeft, objRight, objMidle;
  BOOL has_collision = FALSE;

  ecs_raw(gEcs, ENABLE_TILE_COLLISION_TAG, &entities, NULL, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    ecs_fill(gEcs, entities[i], _componentIds, 2, (void**)&components);
    objBottom = get_obj_bottom(components);
    objLeft   = get_obj_left(components);
    objRight  = get_obj_right(components);
    objMidle  = (objLeft + objRight) / 2;

    has_collision = check(objLeft, objBottom, objLeft / TILE_SIZE, objBottom / TILE_SIZE);
    has_collision = has_collision ||
                    check(objMidle, objBottom, objMidle / TILE_SIZE, objBottom / TILE_SIZE);
    has_collision = has_collision ||
                    check(objRight, objBottom, objRight / TILE_SIZE, objBottom / TILE_SIZE);
    if (has_collision)
      components.transform->position = components.transform->lastPosition;
  }
}

static BOOL check_midle(int objX, int objY, int cellX, int cellY)
{
  POINT  objPos        = { objX, objY };
  RECT   midleCollider = { cellX * TILE_SIZE, cellY * TILE_SIZE + 12, TILE_SIZE, 4 };
  tile_t adjLeft, adj_top, adj_right;
  BOOL   collision = FALSE;
  adjLeft         = map_at(MAP_LAYER_WALL, cellX - 1, cellY);
  adj_top          = map_at(MAP_LAYER_WALL, cellX, cellY - 1);
  adj_right        = map_at(MAP_LAYER_WALL, cellX + 1, cellY);
  if (!has_collider(adjLeft) && has_collider(adj_top))
  {
    RECT left_collider = { cellX * TILE_SIZE, cellY * TILE_SIZE, 4, TILE_SIZE };
    collision          = SDL_PointInRect(&objPos, &left_collider);
  }
  if (!has_collider(adj_right) && has_collider(adj_top))
  {
    RECT right_collider = { cellX * TILE_SIZE + 12, cellY * TILE_SIZE, 4, TILE_SIZE };
    collision           = collision || SDL_PointInRect(&objPos, &right_collider);
  }
  return collision || SDL_PointInRect(&objPos, &midleCollider);
}

static BOOL check_right(int objX, int objY, int cellX, int cellY)
{
  tile_t adjTop   = map_at(MAP_LAYER_WALL, cellX, cellY - 1);
  tile_t adjRight = map_at(MAP_LAYER_WALL, cellX - 1, cellY);

  if (!has_collider(adjTop) && has_collider(adjRight))
    return FALSE;
  POINT objPos  = { objX, objY };
  RECT  collider = { cellX * TILE_SIZE, cellY * TILE_SIZE, 4, TILE_SIZE };
  return SDL_PointInRect(&objPos, &collider);
}

static BOOL check_left(int obj_x, int obj_y, int cell_x, int cell_y)
{
  tile_t adjTop  = map_at(MAP_LAYER_WALL, cell_x, cell_y - 1);
  tile_t adjLeft = map_at(MAP_LAYER_WALL, cell_x + 1, cell_y);

  if (!has_collider(adjTop) && has_collider(adjLeft))
    return FALSE;

  POINT objPos  = { obj_x, obj_y };
  RECT  collider = { cell_x * TILE_SIZE + 12, cell_y * TILE_SIZE, 4, TILE_SIZE };
  return SDL_PointInRect(&objPos, &collider);
}

#if DEBUG
static const COLOR _collider_color = { 0x00, 0xff, 0x00, 0xff };

static void draw_midle(int cell_x, int cell_y)
{
  tile_t adj_left, adj_top, adj_right;
  adj_left  = map_at(MAP_LAYER_WALL, cell_x - 1, cell_y);
  adj_top   = map_at(MAP_LAYER_WALL, cell_x, cell_y - 1);
  adj_right = map_at(MAP_LAYER_WALL, cell_x + 1, cell_y);
  if (!has_collider(adj_left) && has_collider(adj_top))
    draw_right(cell_x, cell_y);
  if (!has_collider(adj_right) && has_collider(adj_top))
    draw_left(cell_x, cell_y);

  RECT collider = { cell_x * TILE_SIZE - gViewport.x,
                    cell_y * TILE_SIZE + 12 - gViewport.y,
                    TILE_SIZE,
                    4 };
  SDL_SetRenderDrawColor(gRenderer,
                         _collider_color.r,
                         _collider_color.g,
                         _collider_color.b,
                         _collider_color.a);
  SDL_RenderDrawRect(gRenderer, &collider);
}

static void draw_left(int cell_x, int cell_y)
{

  tile_t adj_top  = map_at(MAP_LAYER_WALL, cell_x, cell_y - 1);
  tile_t adj_left = map_at(MAP_LAYER_WALL, cell_x + 1, cell_y);

  if (!has_collider(adj_top) && has_collider(adj_left))
    return;

  RECT collider = { cell_x * TILE_SIZE - gViewport.x + 12,
                    cell_y * TILE_SIZE - gViewport.y,
                    4,
                    TILE_SIZE };
  SDL_SetRenderDrawColor(gRenderer,
                         _collider_color.r,
                         _collider_color.g,
                         _collider_color.b,
                         _collider_color.a);
  SDL_RenderDrawRect(gRenderer, &collider);
}
static void draw_right(int cell_x, int cell_y)
{
  tile_t adj_top   = map_at(MAP_LAYER_WALL, cell_x, cell_y - 1);
  tile_t adj_right = map_at(MAP_LAYER_WALL, cell_x - 1, cell_y);

  if (!has_collider(adj_top) && has_collider(adj_right))
    return;
  RECT collider = { cell_x * TILE_SIZE - gViewport.x,
                    cell_y * TILE_SIZE - gViewport.y,
                    4,
                    TILE_SIZE };
  SDL_SetRenderDrawColor(gRenderer,
                         _collider_color.r,
                         _collider_color.g,
                         _collider_color.b,
                         _collider_color.a);
  SDL_RenderDrawRect(gRenderer, &collider);
}
#endif
