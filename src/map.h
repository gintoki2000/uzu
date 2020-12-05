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

#endif // MAP_H
