#include "ui_helper.h"

extern SDL_Renderer* gRenderer;

void draw_bordered_box(const SDL_Rect* rect, SDL_Color background_color, SDL_Color border_color)
{
  SDL_SetRenderDrawColor(gRenderer,
                         background_color.r,
                         background_color.g,
                         background_color.b,
                         background_color.a);
  SDL_RenderFillRect(gRenderer, rect);
  SDL_SetRenderDrawColor(gRenderer,
                         border_color.r,
                         border_color.g,
                         border_color.b,
                         border_color.a);
  SDL_RenderDrawRect(gRenderer,
                     &(RECT){
                         rect->x + 1,
                         rect->y + 1,
                         rect->w - 2,
                         rect->h - 2,
                     });
}
