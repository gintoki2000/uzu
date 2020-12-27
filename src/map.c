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

extern SDL_Rect g_viewport;
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
      (AnimatedTile){
          .tiles    = { 10, 11, 12, 13 },
          .cnt      = 4,
          .duration = 40,
      },
  [ANIMATED_TILE_BLUE_FOUNTAIN_MID] =
      (AnimatedTile){
          .tiles    = { 35, 36, 37 },
          .cnt      = 3,
          .duration = 20,
      },
  [ANIMATED_TILE_BLUE_FOUNTAIN_BASIN] =
      (AnimatedTile){
          .tiles    = { 29, 30, 31 },
          .cnt      = 3,
          .duration = 20,
      },
  [ANIMATED_TILE_RED_FOUNTAIN_MID] =
      (AnimatedTile){
          .tiles    = { 38, 39, 40 },
          .cnt      = 3,
          .duration = 20,

      },
  [ANIMATED_TILE_RED_FOUNTAIN_BASIN] =
      (AnimatedTile){
          .tiles    = { 32, 33, 34 },
          .cnt      = 3,
          .duration = 20,
      },
};

static s32* _layers[] = {
  [MAP_LAYER_FLOOR] = _floor_layer,
  [MAP_LAYER_WALL]  = _wall_layer,
  [MAP_LAYER_FRONT] = _front_layer,
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

void map_update_animated_cells()
{
  _ticks++;
  int                 idx;
  const AnimatedTile* tile;
  for (int i = 0; i < _animated_cell_list.cnt; ++i)
  {

    tile = _animated_cell_list.cells[i].anim_tile;
    idx  = _ticks / tile->duration;
    idx  = idx % tile->cnt;
    map_set_tile_at(_animated_cell_list.cells[i].layer,
                _animated_cell_list.cells[i].col,
                _animated_cell_list.cells[i].row,
                tile->tiles[idx]);
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

  start_x = MAX(0, start_x);
  start_y = MAX(0, start_y);

  end_x = MIN(end_x, _col_cnt);
  end_y = MIN(end_y, _row_cnt);

  dst.w = dst.h = TILE_SIZE;
  src.w = src.h = TILE_SIZE;
  src.y         = 0;

  tileset = get_texture(TEX_TILESET);

  for (s32 y = start_y; y < end_y; ++y)
  {
    begin_row = y * _col_cnt;
    for (s32 x = start_x; x < end_x; ++x)
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
  SDL_assert(w < MAP_MAX_COL);
  SDL_assert(h < MAP_MAX_ROW);
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
  for (int i = 0; i < cnt; ++i)
  {
    if (data[i] == MAP_TRAP_TILE)
      map_create_animated_cell(layer, i / _col_cnt, i % _col_cnt, ANIMATED_TILE_TRAP);
    else if (data[i] == MAP_BLUE_FOUNTAIN_MID_TILE)
      map_create_animated_cell(layer, i / _col_cnt, i % _col_cnt, ANIMATED_TILE_BLUE_FOUNTAIN_MID);
    else if (data[i] == MAP_BLUE_FOUNTAIN_BASIN_TILE)
      map_create_animated_cell(layer,
                               i / _col_cnt,
                               i % _col_cnt,
                               ANIMATED_TILE_BLUE_FOUNTAIN_BASIN);
    else if (data[i] == MAP_RED_FOUNTAIN_MID_TILE)
      map_create_animated_cell(layer, i / _col_cnt, i % _col_cnt, ANIMATED_TILE_RED_FOUNTAIN_MID);
    else if (data[i] == MAP_RED_FOUNTAIN_BASIN_TILE)
      map_create_animated_cell(layer, i / _col_cnt, i % _col_cnt, ANIMATED_TILE_RED_FOUNTAIN_BASIN);
  }
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

void map_clear(s32 layer)
{
  memset(_layers[layer], 0, _row_cnt * _col_cnt);
}
