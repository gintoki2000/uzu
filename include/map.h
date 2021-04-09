#ifndef MAP_H
#define MAP_H

#include "toolbox/toolbox.h"

#define MAP_MAX_ROW 300
#define MAP_MAX_COL 300
#define MAP_MAX_TILES (MAP_MAX_ROW * MAP_MAX_COL)

typedef int tile_t;

enum
{
  MAP_LAYER_FOOR,
  MAP_LAYER_WALL
};

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

void   map_update_animated_cells();
void   map_scan_animated_cells(int layer);
void   map_draw(void);
void   map_set_size(int w, int h);
void   map_get_size(int* w, int* h);
void   map_load(const tile_t* floor, const tile_t* wall, int width, int height);
void   map_clear();
void   map_set(int layer, int cell_x, int cell_y, tile_t tile);
tile_t map_at(int layer, int cell_x, int cell_y);

INLINE tile_t map_wall_at(int cell_x, int cell_y) 
{ return map_at(MAP_LAYER_WALL, cell_x, cell_y); }

INLINE tile_t map_floor_at(int cell_x, int cell_y)
{ return map_at(MAP_LAYER_FOOR, cell_x, cell_y); }

#endif // MAP_H
