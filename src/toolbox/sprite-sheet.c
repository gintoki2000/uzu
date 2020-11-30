#include <stdlib.h>
#include "sprite-sheet.h"
SpriteSheet* sprite_sheet_init_1(SpriteSheet* self,
                                 SDL_Texture* tex,
                                 u32          x,
                                 u32          y,
                                 u32          w,
                                 u32          h,
                                 u32          row_count,
                                 u32          col_count)
{
  SDL_Rect rect;
  self->sprites = malloc(row_count * col_count * sizeof(Sprite));
  self->count = row_count * col_count;

  for (u32 i = 0; i < row_count; ++i)
  {
    for (u32 j = 0; j < col_count; ++j)
    {
      rect.x = j * w + x;
      rect.y = i * h + y;
      rect.w = w;
      rect.h = h;
      sprite_init_w_rect(&self->sprites[i * col_count + j], tex, &rect);
    }
  }
  return self;
}
