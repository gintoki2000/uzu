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

#define DEFINE_SCENE(name)                                                                         \
  static void on_load(void);                                                                       \
  static void on_unload(void);                                                                     \
  static void on_update(void);                                                                     \
  static void on_event(const SDL_Event*);                                                          \
  const Scene g_##name##_scene = { .on_load   = on_load,                                           \
                                   .on_unload = on_unload,                                         \
                                   .on_update = on_update,                                         \
                                   .on_event  = on_event }

#define DECLARE_SCENE(name) extern const Scene g_##name##_scene

DECLARE_SCENE(game);
DECLARE_SCENE(title);
DECLARE_SCENE(main);
DECLARE_SCENE(select_character);
#endif // SCENE_H
