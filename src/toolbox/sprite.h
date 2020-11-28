#ifndef SPRITE_H
#define SPRITE_H
#include <common.h>
#include <SDL2/SDL.h>
typedef struct
{
  SDL_Texture* tex;
  SDL_Rect     rect;
} Sprite;
Sprite* sprite_init(Sprite* s, SDL_Texture* tex);
Sprite* sprite_init_w_rect(Sprite* s, SDL_Texture* tex, const SDL_Rect* rect);
#endif // SPRITE_H
