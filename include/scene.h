#ifndef SCENE_H
#define SCENE_H
#include "SDL.h"

typedef struct Scene
{
  void (*on_load)(void);
  void (*on_unload)(void);
  void (*on_update)(void);
  void (*on_event)(const SDL_Event*);
} Scene;

extern const Scene g_game_scene;
extern const Scene g_title_scene;
extern const Scene g_main_menu;
#endif // SCENE_H
