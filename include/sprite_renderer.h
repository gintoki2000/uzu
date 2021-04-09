#ifndef SPRITE_RENDERER_H
#define SPRITE_RENDERER_H
#include "global.h"
void sprite_renderer_init();

void sprite_renderer_draw_ex(Sprite sprite,
                             POINT  position,
                             POINT  center,
                             double rotation,
                             FLIP   flip,
                             COLOR  color,
                             int    depth);

void sprite_renderer_draw(Sprite sprite, POINT position, int depth);
void sprite_renderer_begin(void);
void sprite_renderer_end(void);

#endif // SPRITE_RENDERER_H
