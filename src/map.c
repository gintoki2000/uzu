#include "map.h"
#include "constances.h"
#include "resources.h"
#include <SDL2/SDL_render.h>
#include <string.h>

enum
{
  TILE_TYPE_FLOOR,
  TILE_TYPE_WALL,
  NUM_TILE_TYPES
};

extern SDL_Rect g_viewport;

static s32 _floor_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _wall_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _front_layer[MAP_MAX_ROW * MAP_MAX_COL];
static s32 _row_cnt;
static s32 _col_cnt;
static s32 _position_x;
static s32 _position_y;

static s32* _layers[] = {
  [MAP_LAYER_FLOOR] = _floor_layer,
  [MAP_LAYER_WALL]  = _wall_layer,
  [MAP_LAYER_FRONT] = _front_layer,
};

static BOOL _is_layer_enable[NUM_MAP_LAYERS];

void map_draw_layer(int layer, SDL_Renderer* renderer)
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
      dst.x = x * TILE_SIZE + _position_x;
      dst.y = y * TILE_SIZE + _position_y;

      tile_id = data[begin_row + x];
      if (tile_id == 0)
        continue;
      tile_id = tile_id - 1;

      src.x = tile_id * TILE_SIZE;

      SDL_RenderCopy(renderer, tileset, &src, &dst);
    }
  }
}

void map_draw(SDL_Renderer* renderer)
{
  for (s32 i = 0; i < NUM_MAP_LAYERS; ++i)
  {
    if (_is_layer_enable[i])
    {
      map_draw_layer(_layers[i], renderer);
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
}

s32* map_get_layer(s32 layer) { return _layers[layer]; }

void map_disable_layer(s32 layer) { _is_layer_enable[layer] = false; }

void map_enable_layer(s32 layer) { _is_layer_enable[layer] = true; }

BOOL map_is_layer_enable(s32 layer) { return _is_layer_enable[layer]; }

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
