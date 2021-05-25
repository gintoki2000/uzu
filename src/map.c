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

extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;

static tile_t  _floorLayer[MAP_MAX_TILES];
static tile_t  _wallLayer[MAP_MAX_TILES];
static s32     _depth[MAP_MAX_TILES];
static s32     _rowCount;
static s32     _columnCount;
static s32     _ticks;
static tile_t* _layerTbl[] = { _floorLayer, _wallLayer };
static struct
{
  AnimatedCell cells[MAP_MAX_ANIMATED_CELLS];
  int          cnt;
} _animatedCells;

enum
{
  ANIMATED_TILE_TRAP,
  ANIMATED_TILE_RED_FOUNTAIN_MID,
  ANIMATED_TILE_RED_FOUNTAIN_BASIN,
  ANIMATED_TILE_BLUE_FOUNTAIN_MID,
  ANIMATED_TILE_BLUE_FOUNTAIN_BASIN,
  NUM_ANIMATED_TILES,
};

static AnimatedTile _animatedTileTbl[NUM_ANIMATED_TILES] = {
  [ANIMATED_TILE_TRAP]                = { .tiles = { 10, 11, 12, 13 }, .cnt = 4, .duration = 40 },
  [ANIMATED_TILE_BLUE_FOUNTAIN_MID]   = { .tiles = { 35, 36, 37 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_BLUE_FOUNTAIN_BASIN] = { .tiles = { 29, 30, 31 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_RED_FOUNTAIN_MID]    = { .tiles = { 38, 39, 40 }, .cnt = 3, .duration = 20 },
  [ANIMATED_TILE_RED_FOUNTAIN_BASIN]  = { .tiles = { 32, 33, 34 }, .cnt = 3, .duration = 20 },
};

void map_create_animated_cell(int layer, int row, int col, int anim_tile_id)
{
  if (_animatedCells.cnt < MAP_MAX_ANIMATED_CELLS)
  {
    _animatedCells.cells[_animatedCells.cnt++] = (AnimatedCell){
      .row       = row,
      .col       = col,
      .anim_tile = &_animatedTileTbl[anim_tile_id],
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
    tile       = &_animatedTileTbl[i];
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

  data     = _layerTbl[layer];
  tile_cnt = _rowCount * _columnCount;

  for (i = 0; i < tile_cnt; ++i)
  {
    animated_tile_id = to_animated_tile_id(data[i]);

    if (animated_tile_id != -1)
    {
      col = i % _columnCount;
      row = i / _columnCount;
      map_create_animated_cell(layer, row, col, animated_tile_id);
    }
  }
}

void map_update_animated_cells()
{
  update_animated_tiles();
  const AnimatedTile* tile;
  const AnimatedCell* cell;
  for (int i = 0; i < _animatedCells.cnt; ++i)
  {
    cell = &_animatedCells.cells[i];
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
#define T(x, y) (((x) < _columnCount && (y) < _rowCount) ? _wallLayer[(y)*_columnCount + (x)] : 0)
#define D(x, y) (_depth[(y)*_columnCount + (x)])
  for (int y = _rowCount - 1; y >= 0; --y)
    for (int x = 0; x < _columnCount; ++x)
    {
      tile = T(x, y);
      if (tile != 0 && is_roof_tile(tile) && y < _rowCount - 1)
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

  _rowCount = height;
  _columnCount = width;

  cnt = width * height;
  SDL_memcpy(_floorLayer, floor, cnt * sizeof(tile_t));
  SDL_memcpy(_wallLayer, wall, cnt * sizeof(tile_t));
  create_animated_cells(MAP_LAYER_FOOR);
  create_animated_cells(MAP_LAYER_WALL);
  construct_depth_buff();
}

void draw_floor()
{
  s32 start_x, start_y, end_x, end_y;

  start_x = gViewport.x / TILE_SIZE;
  start_y = gViewport.y / TILE_SIZE;

  end_x = (gViewport.x + gViewport.w) / TILE_SIZE;
  end_y = (gViewport.y + gViewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, _columnCount - 1);
  end_y = min(end_y, _rowCount - 1);

  Sprite sprite;
  POINT  position;
  tile_t tile;
  sprite.textureId = TEX_TILESET;
  sprite.rect.w     = TILE_SIZE;
  sprite.rect.h     = TILE_SIZE;
  sprite.rect.y     = 0;

  for (int y = start_y; y <= end_y; ++y)
    for (int x = start_x; x <= end_x; ++x)
    {
      tile = _floorLayer[x + y * _columnCount];
      if (tile == 0)
        continue;
      sprite.rect.x = (tile - 1) * TILE_SIZE;
      position.x    = x * TILE_SIZE - gViewport.x;
      position.y    = y * TILE_SIZE - gViewport.y;
      sprite_renderer_draw(sprite, position, -999);
    }
}

static void draw_wall(void)
{
  s32 start_x, start_y, end_x, end_y;

  start_x = gViewport.x / TILE_SIZE;
  start_y = gViewport.y / TILE_SIZE;

  end_x = (gViewport.x + gViewport.w) / TILE_SIZE;
  end_y = (gViewport.y + gViewport.h) / TILE_SIZE;

  start_x = max(0, start_x);
  start_y = max(0, start_y);

  end_x = min(end_x, _columnCount - 1);
  end_y = min(end_y, _rowCount - 1);

  Sprite sprite;
  POINT  position;
  tile_t tile;
  sprite.textureId = TEX_TILESET;
  sprite.rect.w     = TILE_SIZE;
  sprite.rect.h     = TILE_SIZE;
  sprite.rect.y     = 0;

#define T(x, y) (_wallLayer[(y)*_columnCount + (x)])
#define D(x, y) (_depth[(y)*_columnCount + (x)])
  for (int y = start_y; y <= end_y; ++y)
    for (int x = start_x; x <= end_x; ++x)
    {
      tile = T(x, y);
      if (tile == 0)
        continue;
      sprite.rect.x = (tile - 1) * TILE_SIZE;
      position.x    = x * TILE_SIZE - gViewport.x;
      position.y    = y * TILE_SIZE - gViewport.y;
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
  _rowCount = h;
  _columnCount = w;
}

void map_get_size(s32* w, s32* h)
{
  if (w != NULL)
    *w = _columnCount;
  if (h != NULL)
    *h = _rowCount;
}

void map_clear()
{
  _animatedCells.cnt  = 0;
  _columnCount        = 0;
  _rowCount           = 0;
  SDL_memset(_floorLayer, 0, sizeof(_floorLayer));
  SDL_memset(_wallLayer, 0, sizeof(_wallLayer));
}

tile_t map_at(int layer, int cell_x, int cell_y)
{
  if (cell_x < 0 || cell_x >= _columnCount)
    return 0;
  if (cell_y < 0 || cell_y >= _rowCount)
    return 0;
  return _layerTbl[layer][cell_x + cell_y * _columnCount];
}

void map_set(int layer, int cell_x, int cell_y, tile_t tile)
{
  if (cell_x < 0 || cell_x >= _columnCount)
    return;
  if (cell_y < 0 || cell_y >= _rowCount)
    return;
  _layerTbl[layer][cell_x + cell_y * _columnCount] = tile;
}
