#include <toolbox/sprite.h>
#include <SDL2/SDL_render.h>

Sprite* sprite_init(Sprite* s, SDL_Texture* tex)
{
  s->tex = tex;
  s->rect.x = 0;
  s->rect.y = 0;
  SDL_QueryTexture(tex, NULL, NULL, &s->rect.w, &s->rect.h);
  return s;
}

Sprite* sprite_init_w_rect(Sprite* s, SDL_Texture* tex, const SDL_Rect* rect)
{
	s->rect = *rect;
	s->tex = tex;
	return s;
}
