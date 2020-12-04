#include "map.h"
#include "constances.h"
#include <SDL2/SDL_render.h>
#include <string.h>

enum
{
  TILE_TYPE_FLOOR,
  TILE_TYPE_WALL,
  NUM_TILE_TYPES
};

extern SDL_Rect      g_viewport;

static int _floor_layer[MAP_MAX_ROW * MAP_MAX_COL];
static int _wall_layer[MAP_MAX_ROW * MAP_MAX_COL];
static int _row_cnt;
static int _col_cnt;
static int _position_x;
static int _position_y;

static int* _layers[] = {
  [MAP_LAYER_FLOOR] = _floor_layer, 
  [MAP_LAYER_WALL] = _wall_layer
};

static bool _is_layer_enable[NUM_MAP_LAYERS];

static void map_draw_layer(const int* data, SDL_Renderer* renderer)
{
  int          start_x, start_y, end_x, end_y;
  SDL_Texture* tileset;
  SDL_Rect     dst;
  SDL_Rect     src;
  int          tile_id, begin_row;

  start_x = g_viewport.x / TILE_SIZE;
  start_y = g_viewport.y / TILE_SIZE;

  end_x = (g_viewport.x + g_viewport.w) / TILE_SIZE;
  end_y = (g_viewport.y + g_viewport.h) / TILE_SIZE;

  start_x = MAX(0, start_x);
  start_y = MAX(0, start_y);

  end_x = MIN(end_x, _col_cnt - 1);
  end_y = MIN(end_y, _row_cnt - 1);

  dst.w = dst.h = TILE_SIZE;
  src.w = src.h = TILE_SIZE;
  src.y = 0;

  for (int y = start_y; y < end_y; ++y)
  {
    begin_row = y * _col_cnt;
    for (int x = start_x; x < end_x; ++x)
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
  for (int i = 0; i < NUM_MAP_LAYERS; ++i)
  {
    if (_is_layer_enable[i])
    {
      map_draw_layer(_layers[i], renderer);
    }
  }
}

void map_set_size(int w, int h)
{
  SDL_assert(w < MAP_MAX_COL);
  SDL_assert(h < MAP_MAX_ROW);
  _row_cnt = h;
  _col_cnt = w;
}

void map_get_size(int *w, int *h)
{
  if (w != NULL)
    *w = _col_cnt;
  if (h != NULL)
    *h = _row_cnt;
}

void map_set_data(int layer, const int *data)
{
  memcpy(_layers[layer], data, sizeof(int) * _row_cnt * _col_cnt);
}

int* map_get_layer(int layer)
{
  return _layers[layer];
}

void map_disable_layer(int layer)
{
  _is_layer_enable[layer] = false;
}

void map_enable_layer(int layer)
{
  _is_layer_enable[layer] = true;
}

bool map_is_layer_enable(int layer)
{
  return _is_layer_enable[layer];
}
