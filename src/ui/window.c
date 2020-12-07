#include "window.h"

static const UIVtbl _vtable = (UIVtbl){
  .fini     = (void (*)(UI*))ui_window_fini,
  .on_paint = (void (*)(UI*, SDL_Renderer*))ui_window_on_paint,
};

static const SDL_Rect big_bg            = (SDL_Rect){ 74, 82, 20, 20 };
static const SDL_Rect border_verticle   = (SDL_Rect){ 23, 49, 10, 5 };
static const SDL_Rect border_horizontal = (SDL_Rect){ 17, 55, 5, 10 };
static const SDL_Rect corner            = (SDL_Rect){ 16, 48, 7, 7 };

ui_Window* ui_window_new(int w, int h)
{
  ui_Window* win = malloc(sizeof(ui_Window));
  UI(win)->vtbl  = &_vtable;

  return ui_window_init(win, w, h);
}

ui_Window* ui_window_init(ui_Window* self, int w, int h)
{
  self->w = w;
  self->h = h;
  return self;
}

void ui_window_on_paint(ui_Window* self, SDL_Renderer* renderer) {
  int x, y;
  x = UI(self)->pos.x;
  y = UI(self)->pos.y;

  
}
