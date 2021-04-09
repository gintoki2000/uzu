#include "map.h"
#include "constances.h"
#include "resources.h"
#include "sprite_renderer.h"
#include <string.h>

#define MAP_MAX_ANIMATED_CELLS 200
#define MAP_TRAP_TILE 10
#define MAP_RED_FOUNTAIN_MID_TILE 38
#define MAP_RED_FOUNTAIN_BASIN_TILE 32
#define MAP_BLUE_FOUNTAIN_MID_TILE 35
#define MAP_BLUE_FOUNTAIN_BASIN_TILE 29

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;

static tile_t  _floor_layer[MAP_MAX_TILES];
static tile_t  _wall_layer[MAP_MAX_TILES];
static s32     _depth[MAP_MAX_TILES];
static s32     _row_cnt;
static s32     _col_cnt;
static s32     _ticks;
static tile_t* _layer_tbl[] = { _floor_layer, _wall_layer };
static struct
{
  AnimatedCell cells[MAP_MAX_ANIMATED_CELLS];
  int          cnt;
} _animated_cells;

enum
{
  ANIMATED_TILE_TRAP,
  ANIMATED_TILE_RED_FOUNTAIN_MID,
  ANIMATED_TILE_RED_FOUNTAIN_BASIN,
  ANIMATED_TILE_BLUE_FOUNTAIN_MID,
  ANIMATED_TILE_BLUE_FOUNTAIN_BASIN,
  NUM_ANIMATED_TILES,
};

static AnimatedTile _animated_tile_tbl[NUM_ANIMATED_TILES] = {
  [ANIMATED_TILE_TRAP]                = { .tiles = { 10, 11, 12, 13 }, .cnt = 4, .duration = 40 },
  [ANIMATED_TILE_BLUE_FOUNTAIN_MID]   = { .tiles = { 35, 36, 37 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_BLUE_FOUNTAIN_BASIN] = { .tiles = { 29, 30, 31 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_RED_FOUNTAIN_MID]    = { .tiles = { 38, 39, 40 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_RED_FOUNTAIN_BASIN]  = { .tiles = { 32, 33, 34 }, .cnt = 3, .duration = 20 },
};

void map_create_animated_cell(int layer, int row, int col, int anim_tile_id)
{
  if (_animated_cells.cnt < MAP_MAX_ANIMATED_CELLS)
  {
    _animated_cells.cells[_animated_cells.cnt++] = (AnimatedCell){
      .row       = row,
      .col       = col,
      .anim_tile = &_animated_tile_tbl[anim_tile_id],
      .layer     = layer,
    };
  }
}

static void update_animated_tiles()
{
  AnimatedTile* tile;
  ++_ticks;
  for (int i = 0; i < NUM_ANIMATED_TILES; ++i)
  {
    tile       = &_animated_tile_tbl[i];
    tile->curr = (_ticks / tile->duration) % tile->cnt;
  }
}

static int to_animated_tile_id(s32 tile)
{
  int animated_tile_id = -1;
  switch (tile)
  {
  case MAP_TRAP_TILE:
    animated_tile_id = ANIMATED_TILE_TRAP;
    break;
  case MAP_BLUE_FOUNTAIN_MID_TILE:
    animated_tile_id = ANIMATED_TILE_BLUE_FOUNTAIN_MID;
    break;
  case MAP_BLUE_FOUNTAIN_BASIN_TILE:
    animated_tile_id = ANIMATED_TILE_BLUE_FOUNTAIN_BASIN;
    break;
  case MAP_RED_FOUNTAIN_MID_TILE:
    animated_tile_id = ANIMATED_TILE_BLUE_FOUNTAIN_MID;
    break;
  case MAP_RED_FOUNTAIN_BASIN_TILE:
    animated_tile_id = ANIMATED_TILE_BLUE_FOUNTAIN_BASIN;
    break;
  }
  return animated_tile_id;
}

static void create_animated_cells(int layer)
{
  int        tile_cnt, animated_tile_id;
  int        i;
  int        col;
  int        row;
  const s32* data;

  data     = _layer_tbl[layer];
  tile_cnt = _row_cnt * _col_cnt;

  for (i = 0; i < tile_cnt; ++i)
  {
    animated_tile_id = to_animated_tile_id(data[i]);

    if (animated_tile_id != -1)
    {
      col = i % _col_cnt;
      row = i / _col_cnt;
      map_create_animated_cell(layer, row, col, animated_tile_id);
    }
  }
}

void map_update_animated_cells()
{
  update_animated_tiles();
  const AnimatedTile* tile;
  const AnimatedCell* cell;
  for (int i = 0; i < _animated_cells.cnt; ++i)
  {
    cell = &_animated_cells.cells[i];
    tile = cell->anim_tile;
    map_set(cell->layer, cell->col, cell->row, tile->tiles[tile->curr]);
  }
}

BOOL is_roof_tile(tile_t tile)
{
#define _(x) [x]      = TRUE
  static BOOL lut[65] = { _(19), _(20), _(25), _(26), _(45), _(46), _(49), _(50), _(58),
                          _(59), _(60), _(61), _(62), _(63), _(64), _(40), _(18) };
#undef _
  return lut[tile - 1];
}
static void construct_depth_buff(void)
{
  tile_t tile;
#define T(x, y) (((x) < _col_cnt && (y) < _row_cnt) ? _wall_layer[(y)*_col_cnt + (x)] : 0)
#define D(x, y) (_depth[(y)*_col_cnt + (x)])
  for (int y = _row_cnt - 1; y >= 0; --y)
    for (int x = 0; x < _col_cnt; ++x)
    {
      tile = T(x, y);
      if (tile != 0 && is_roof_tile(tile) && y < _row_cnt - 1)
        D(x, y) = D(x, y + 1);
      else
        D(x, y) = (y + 1) * TILE_SIZE;
    }
#undef T
#undef D
}

void map_load(const tile_t* floor, const tile_t* wall, int width, int height)
{
  int cnt;

  map_clear();

  _row_cnt = height;
  _col_cnt = width;

  cnt = width * height;
  SDL_memcpy(_floor_layer, floor, cnt * sizeof(tile_t));
  SDL_memcpy(_wall_layer, wall, cnt * sizeof(tile_t));
  create_animated_cells(MAP_LAYER_FOOR);
  create_animated_cells(MAP_LAYER_WALL);
  construct_depth_buff();
}

void draw_floor()
{
  s32 start_x, start_y, end_x, end_y;

  start_x = g_viewport.x / TILE_SIZE;
  start_y = g_viewport.y / TILE_SIZE;

  end_x = (g_viewport.x + g_viewport.w) / TILE_SIZE;
  end_y = (g_viewport.y + g_viewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, _col_cnt - 1);
  end_y = min(end_y, _row_cnt - 1);

  Sprite sprite;
  POINT  position;
  tile_t tile;
  sprite.texture_id = TEX_TILESET;
  sprite.rect.w     = TILE_SIZE;
  sprite.rect.h     = TILE_SIZE;
  sprite.rect.y     = 0;

  for (int y = start_y; y <= end_y; ++y)
    for (int x = start_x; x <= end_x; ++x)
    {
      tile = _floor_layer[x + y * _col_cnt];
      if (tile == 0)
        continue;
      sprite.rect.x = (tile - 1) * TILE_SIZE;
      position.x    = x * TILE_SIZE - g_viewport.x;
      position.y    = y * TILE_SIZE - g_viewport.y;
      sprite_renderer_draw(sprite, position, -999);
    }
}

static void draw_wall(void)
{
  s32 start_x, start_y, end_x, end_y;

  start_x = g_viewport.x / TILE_SIZE;
  start_y = g_viewport.y / TILE_SIZE;

  end_x = (g_viewport.x + g_viewport.w) / TILE_SIZE;
  end_y = (g_viewport.y + g_viewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, _col_cnt - 1);
  end_y = min(end_y, _row_cnt - 1);

  Sprite sprite;
  POINT  position;
  tile_t tile;
  sprite.texture_id = TEX_TILESET;
  sprite.rect.w     = TILE_SIZE;
  sprite.rect.h     = TILE_SIZE;
  sprite.rect.y     = 0;

#define T(x, y) (_wall_layer[(y)*_col_cnt + (x)])
#define D(x, y) (_depth[(y)*_col_cnt + (x)])
  for (int y = start_y; y <= end_y; ++y)
    for (int x = start_x; x <= end_x; ++x)
    {
      tile = T(x, y);
      if (tile == 0)
        continue;
      sprite.rect.x = (tile - 1) * TILE_SIZE;
      position.x    = x * TILE_SIZE - g_viewport.x;
      position.y    = y * TILE_SIZE - g_viewport.y;
      sprite_renderer_draw(sprite, position, D(x, y));
    }
#undef D
#undef T
}

void map_draw()
{
  draw_floor();
  draw_wall();
}

void map_set_size(s32 w, s32 h)
{
  SDL_assert(w <= MAP_MAX_COL);
  SDL_assert(h <= MAP_MAX_ROW);
  _row_cnt = h;
  _col_cnt = w;
}

void map_get_size(s32* w, s32* h)
{
  if (w != NULL)
    *w = _col_cnt;
  if (h != NULL)
    *h = _row_cnt;
}

void map_clear()
{
  _animated_cells.cnt = 0;
  _col_cnt            = 0;
  _row_cnt            = 0;
  SDL_memset(_floor_layer, 0, sizeof(_floor_layer));
  SDL_memset(_wall_layer, 0, sizeof(_wall_layer));
}

tile_t map_at(int layer, int cell_x, int cell_y)
{
  if (cell_x < 0 || cell_x >= _col_cnt)
    return 0;
  if (cell_y < 0 || cell_y >= _row_cnt)
    return 0;
  return _layer_tbl[layer][cell_x + cell_y * _col_cnt];
}

void map_set(int layer, int cell_x, int cell_y, tile_t tile)
{
  if (cell_x < 0 || cell_x >= _col_cnt)
    return;
  if (cell_y < 0 || cell_y >= _row_cnt)
    return;
  _layer_tbl[layer][cell_x + cell_y * _col_cnt] = tile;
}
