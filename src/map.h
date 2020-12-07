#ifndef MAP_H
#define MAP_H

#include <toolbox/toolbox.h>

#define MAP_MAX_ROW 300
#define MAP_MAX_COL 300
#define MAP_MAX_TILES MAP_MAX_ROW* MAP_MAX_COL

enum
{
  MAP_LAYER_FLOOR,
  MAP_LAYER_WALL,
  MAP_LAYER_FRONT,
  NUM_MAP_LAYERS
};

#define TILE_WALL_1 53
#define TILE_ROOF_1 51
#define TILE_ROOF_LEFT 58
#define TILE_ROOF_RIGHT 57
#define TILE_LEFT_CORNER_WALL 48
#define TILE_RIGHT_CORNER_WALL 49
#define TILE_LEFT_CORNER_ROOF 46
#define TILE_RIGHT_CORNER_ROOF 47
#define TILE_LEFT_SMALL_WALL 55
#define TILE_RIGHT_SMALL_WALL 56
#define TILE_LEFT_WALL_W_ROOF 24
#define TILE_RIGHT_WALL_W_ROOF 25

void map_draw(SDL_Renderer* renderer);
void map_draw_layer(int layer, SDL_Renderer* renderer);
void map_set_size(s32 w, s32 h);
void map_set_data(s32 layer, const s32* data, u32 cnt);
void map_get_size(s32* w, s32* h);
s32* map_get_layer(s32 layer);
void map_enable_layer(s32 layer);
void map_disable_layer(s32 layer);
BOOL map_is_layer_enable(s32 layer);
BOOL map_is_wall(s32 cell_x, s32 cell_y);
BOOL map_is_floor(s32 cell_x, s32 cell_y);
s32  map_tile_at(s32 layer, s32 x, s32 y);
void set_tile_at(s32 layer, s32 x, s32 y, s32 tile);
void map_clear(s32 layer);

#endif // MAP_H
