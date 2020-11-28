#ifndef SPRITE_SHEET_H
#define SPRITE_SHEET_H
#include <SDL2/SDL_render.h>
#include <common.h>
#include <toolbox/sprite.h>

typedef struct
{
  Sprite* sprites;
  u32     count;
} SpriteSheet;
SpriteSheet* sprite_sheet_init_w_array(SpriteSheet* self, const Sprite* sprites, u32 count);
SpriteSheet* sprite_sheet_init_1(SpriteSheet* self,
                                 SDL_Texture* tex,
                                 u32          x,
                                 u32          y,
                                 u32          w,
                                 u32          h,
                                 u32          row_count,
                                 u32          col_count);
INLINE SpriteSheet* sprite_sheet_init_2(SpriteSheet* self,
                                        SDL_Texture* tex,
                                        u32          w,
                                        u32          h,
                                        u32          row_count,
                                        u32          col_count);
INLINE SpriteSheet* sprite_sheet_init_3(SpriteSheet* self, SDL_Texture* tex, u32 w, u32 h);

INLINE SpriteSheet*
       sprite_sheet_init_2(SpriteSheet* self, SDL_Texture* tex, u32 w, u32 h, u32 row_count, u32 col_count)
{
  return sprite_sheet_init_1(self, tex, 0, 0, w, h, row_count, col_count);
}

INLINE SpriteSheet* sprite_sheet_init_3(SpriteSheet* self, SDL_Texture* tex, u32 w, u32 h)
{
  int tw, th;
  SDL_QueryTexture(tex, NULL, NULL, &tw, &th);
  u32 row_count = th / h;
  u32 col_count = tw / w;
  return sprite_sheet_init_2(self, tex, w, h, row_count, col_count);
}

INLINE void sprite_sheet_fini(SpriteSheet* self)
{
  free(self->sprites);
  self->sprites = NULL;
}
#endif // SPRITE_SHEET_H
