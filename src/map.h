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
  NUM_MAP_LAYERS
};

void map_draw();
void map_set_size(s32 w, s32 h);
void map_set_data(s32 layer, const s32* data);
void map_get_size(s32* w, s32* h);
s32* map_get_layer(s32 layer);
void map_enable_layer(s32 layer);
void map_disable_layer(s32 layer);
bool map_is_layer_enable(s32 layer);
bool map_is_wall(s32 cell_x, s32 cell_y);
bool map_is_floor(s32 cell_x, s32 cell_y);

#endif // MAP_H
