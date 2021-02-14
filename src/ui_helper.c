#include "ui_helper.h"

extern SDL_Renderer* g_renderer;

void draw_bordered_box(const SDL_Rect* rect, SDL_Color background_color, SDL_Color border_color)
{
  SDL_SetRenderDrawColor(g_renderer,
                         background_color.r,
                         background_color.g,
                         background_color.b,
                         background_color.a);
  SDL_RenderFillRect(g_renderer, rect);
  SDL_SetRenderDrawColor(g_renderer,
                         border_color.r,
                         border_color.g,
                         border_color.b,
                         border_color.a);
  SDL_RenderDrawRect(g_renderer,
                     &(RECT){
                         rect->x + 1,
                         rect->y + 1,
                         rect->w - 2,
                         rect->h - 2,
                     });
}
