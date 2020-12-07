#ifndef WINDOW_H
#define WINDOW_H
#include "ui.h"
typedef struct ui_Window ui_Window;

struct ui_Window
{
  UI          _parent_instance;
  const char* title;
  int         w;
  int         h;
};

ui_Window* ui_window_new(int w, int h);

ui_Window* ui_window_init(ui_Window* self, int w, int h);
void       ui_window_fini(ui_Window* self);

void ui_window_on_paint(ui_Window* self, SDL_Renderer* renderer);

#endif // WINDOW_H
