#include "map.h"
#include "constances.h"
#include "resources.h"
#include <SDL2/SDL_render.h>
#include <string.h>

enum
{
  ANIMATED_TILE_TRAP,
  ANIMATED_TILE_RED_FOUNTAIN_MID,
  ANIMATED_TILE_RED_FOUNTAIN_BASIN,
  ANIMATED_TILE_BLUE_FOUNTAIN_MID,
  ANIMATED_TILE_BLUE_FOUNTAIN_BASIN,
  ANIMATED_TILE_CNT,
};

#define MAP_MAX_ANIMATED_CELLS 200
#define MAP_TRAP_TILE 10
#define MAP_RED_FOUNTAIN_MID_TILE 38
#define MAP_RED_FOUNTAIN_BASIN_TILE 32
#define MAP_BLUE_FOUNTAIN_MID_TILE 35
#define MAP_BLUE_FOUNTAIN_BASIN_TILE 29

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;

static s32 _floor_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _wall_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _front_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _row_cnt;
static s32 _col_cnt;
static s32 _position_x;
static s32 _position_y;
static s32 _ticks;
static struct
{
  AnimatedCell cells[MAP_MAX_ANIMATED_CELLS];
  int          cnt;
} _animated_cell_list;

static AnimatedTile _animated_tile_list[ANIMATED_TILE_CNT] = {
  [ANIMATED_TILE_TRAP] =
      {
          .tiles    = { 10, 11, 12, 13 },
          .cnt      = 4,
          .duration = 40,
      },
  [ANIMATED_TILE_BLUE_FOUNTAIN_MID] =
      {
          .tiles    = { 35, 36, 37 },
          .cnt      = 3,
          .duration = 20,
      },
  [ANIMATED_TILE_BLUE_FOUNTAIN_BASIN] =
      {
          .tiles    = { 29, 30, 31 },
          .cnt      = 3,
          .duration = 20,
      },
  [ANIMATED_TILE_RED_FOUNTAIN_MID] =
      {
          .tiles    = { 38, 39, 40 },
          .cnt      = 3,
          .duration = 20,

      },
  [ANIMATED_TILE_RED_FOUNTAIN_BASIN] =
      {
          .tiles    = { 32, 33, 34 },
          .cnt      = 3,
          .duration = 20,
      },
};

static s32* _layers[] = {
  [MAP_LAYER_FLOOR]      = _floor_layer,
  [MAP_LAYER_BACK_WALL]  = _wall_layer,
  [MAP_LAYER_FRONT_WALL] = _front_layer,
};

const TileInfo g_tile_info_tbl[] = {
  { TILE_TYPE_FLOOR }, // 0
  { TILE_TYPE_FLOOR }, // 1
  { TILE_TYPE_FLOOR }, // 2
  { TILE_TYPE_FLOOR }, // 3
  { TILE_TYPE_FLOOR }, // 4
  { TILE_TYPE_FLOOR }, // 5
  { TILE_TYPE_FLOOR }, // 6
  { TILE_TYPE_FLOOR }, // 7
  { TILE_TYPE_FLOOR }, // 8
  { TILE_TYPE_FLOOR }, // 9
  { TILE_TYPE_TRAP },  // 10
  { TILE_TYPE_TRAP },  // 11
  { TILE_TYPE_TRAP },  // 12
};

void map_create_animated_cell(int layer, int row, int col, int anim_tile_id)
{
  if (_animated_cell_list.cnt < MAP_MAX_ANIMATED_CELLS)
  {
    _animated_cell_list.cells[_animated_cell_list.cnt++] = (AnimatedCell){
      .row       = row,
      .col       = col,
      .anim_tile = &_animated_tile_list[anim_tile_id],
      .layer     = layer,
    };
  }
}

static void update_animated_tiles()
{
  AnimatedTile* tile;
  ++_ticks;
  for (int i = 0; i < ANIMATED_TILE_CNT; ++i)
  {
    tile = &_animated_tile_list[i];

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

void map_scan_animated_cells(int layer)
{
  int        tile_cnt, animated_tile_id;
  int        i;
  int        col;
  int        row;
  const s32* data;

  data     = _layers[layer];
  tile_cnt = _row_cnt * _col_cnt;

  for (i = 0; i < tile_cnt; ++i)
  {
    animated_tile_id = to_animated_tile_id(data[i]);
    col              = i % _col_cnt;
    row              = i / _col_cnt;

    if (animated_tile_id != -1)
      map_create_animated_cell(layer, row, col, animated_tile_id);
  }
}

void map_update_animated_cells()
{
  update_animated_tiles();
  const AnimatedTile* tile;
  const AnimatedCell* cell;
  for (int i = 0; i < _animated_cell_list.cnt; ++i)
  {

    cell = &_animated_cell_list.cells[i];
    tile = cell->anim_tile;
    map_set_tile_at(cell->layer, cell->col, cell->row, tile->tiles[tile->curr]);
  }
}

void map_draw(int layer)
{
  s32          start_x, start_y, end_x, end_y;
  SDL_Texture* tileset;
  SDL_Rect     dst;
  SDL_Rect     src;
  s32          tile_id, begin_row;
  const s32*   data = _layers[layer];

  start_x = g_viewport.x / TILE_SIZE;
  start_y = g_viewport.y / TILE_SIZE;

  end_x = (g_viewport.x + g_viewport.w) / TILE_SIZE;
  end_y = (g_viewport.y + g_viewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, _col_cnt);
  end_y = min(end_y, _row_cnt);

  dst.w = dst.h = TILE_SIZE;
  src.w = src.h = TILE_SIZE;
  src.y         = 0;

  tileset = get_texture(TEX_TILESET);

  for (s32 y = start_y; y <= end_y; ++y)
  {
    begin_row = y * _col_cnt;
    for (s32 x = start_x; x <= end_x; ++x)
    {
      dst.x = x * TILE_SIZE + _position_x - g_viewport.x;
      dst.y = y * TILE_SIZE + _position_y - g_viewport.y;

      tile_id = data[begin_row + x];
      if (tile_id == 0)
        continue;
      tile_id = tile_id - 1;

      src.x = tile_id * TILE_SIZE;

      SDL_RenderCopy(g_renderer, tileset, &src, &dst);
    }
  }
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

void map_set_data(s32 layer, const s32* data, u32 cnt)
{
  memcpy(_layers[layer], data, sizeof(s32) * cnt);
}

s32* map_get_layer(s32 layer)
{
  return _layers[layer];
}

s32 map_tile_at(s32 layer, s32 x, s32 y)
{
  if (x < 0 || y < 0 || y >= _row_cnt || x >= _col_cnt)
    return 0;
  return _layers[layer][x + y * _col_cnt];
}

BOOL map_is_floor(s32 cell_x, s32 cell_y)
{
  return map_tile_at(MAP_LAYER_FLOOR, cell_x, cell_y) != 0;
}

void map_set_tile_at(s32 layer, s32 x, s32 y, s32 tile)
{
  _layers[layer][x + y * _col_cnt] = tile;
}

void map_clear()
{
  _animated_cell_list.cnt = 0;
  _col_cnt                = 0;
  _row_cnt                = 0;
  SDL_memset(_floor_layer, 0, sizeof(_floor_layer));
  SDL_memset(_wall_layer, 0, sizeof(_floor_layer));
  SDL_memset(_front_layer, 0, sizeof(_floor_layer));
}
