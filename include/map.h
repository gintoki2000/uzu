#ifndef MAP_H
#define MAP_H

#include "toolbox/toolbox.h"

#define MAP_MAX_ROW 300
#define MAP_MAX_COL 300
#define MAP_MAX_TILES MAP_MAX_ROW* MAP_MAX_COL

typedef enum MapLayer
{
  MAP_LAYER_FLOOR,
  MAP_LAYER_BACK_WALL,
  MAP_LAYER_FRONT_WALL,
  NUM_MAP_LAYERS
} MapLayer;

typedef enum TileType
{
  TILE_TYPE_FLOOR,
  TILE_TYPE_WALL,
  TILE_TYPE_TRAP
} TileType;

typedef struct TileInfo
{
  u16 type;
} TileInfo;

typedef struct AnimatedTile
{
  int tiles[4];
  int duration;
  int cnt;
  int curr;
} AnimatedTile;

typedef struct AnimatedCell
{
  int                 row;
  int                 col;
  int                 layer;
  const AnimatedTile* anim_tile;
} AnimatedCell;

extern const TileInfo g_tile_info_tbl[];

void map_update_animated_cells();
void map_scan_animated_cells(int layer);
void map_draw(int layer);
void map_set_size(int w, int h);
void map_set_data(int layer, const s32* data, u32 cnt);
void map_get_size(int* w, int* h);
s32* map_get_layer(int layer);
BOOL map_is_wall(int cell_x, int cell_y);
BOOL map_is_floor(int cell_x, int cell_y);
s32  map_tile_at(int layer, int x, int y);
void map_set_tile_at(int layer, int x, int y, s32 tile);
void map_clear();

#endif // MAP_H
