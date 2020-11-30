#ifndef UZU_ENGINE_H
#define UZU_ENGINE_H
#include <SDL2/SDL.h>
typedef struct
{
  void* user_data;
  SDL_bool (*init)(void*);
  void (*loop)(void*, SDL_Renderer*);
  void (*fini)(void*);
  void (*event)(void*, const SDL_Event*);
} GameDelegate;

typedef struct
{
  const char* window_title;
  int         window_width;
  int         window_height;
  int         frame_rate;
} GameSetting;

void          engine_run(GameDelegate* game_delegate, const GameSetting* game_setting);
void          engine_stop();
void          engine_set_frame_rate(Uint32 frame_rate);
Uint32        engine_get_frame_rate();
SDL_Renderer* engine_get_renderer();
SDL_Window*   engine_get_window();

#endif // UZU_ENGINE_H
