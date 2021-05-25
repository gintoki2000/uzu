#include "engine/engine.h"
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "engine/keyboard.h"
#include "toolbox/toolbox.h"

static SDL_bool _is_running = SDL_FALSE;
static Uint32   _delay_ticks;
static Uint32   _frame_rate;

SDL_Window*   g_window;
SDL_Renderer* gRenderer;

static SDL_bool init(const GameSetting* setting)
{
  g_window = SDL_CreateWindow(setting->window_title,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              setting->window_width,
                              setting->window_height,
                              SDL_WINDOW_SHOWN);

  if (g_window == NULL)
    return SDL_FALSE;

  gRenderer = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
  if (gRenderer == NULL)
    return SDL_FALSE;
  if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG)
  {
    ERROR("failed to init SDL_image\n");
    return FALSE;
  }
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
  {
    ERROR("open audio failed\n");
    return FALSE;
  }
  SDL_SetRenderDrawBlendMode(gRenderer, SDL_BLENDMODE_BLEND);
  input_init();
  engine_set_frame_rate(setting->frame_rate);
  return SDL_TRUE;
}

void engine_run(GameDelegate* game_delegate, const GameSetting* setting)
{
  void (*loop_fn)(void);
  void (*event_fn)(const SDL_Event*);
  Uint32    elapsedTicks, beginingTick;
  SDL_Event event;

  loop_fn  = game_delegate->loop;
  event_fn = game_delegate->event;

  if (init(setting) && game_delegate->init())
  {
    _is_running = SDL_TRUE;

    while (_is_running)
    {
      beginingTick = SDL_GetTicks();
      while (SDL_PollEvent(&event))
      {
        event_fn(&event);
      }
      input_update();
      SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
      SDL_RenderClear(gRenderer);
      loop_fn();
      SDL_RenderPresent(gRenderer);
      elapsedTicks = SDL_GetTicks() - beginingTick;
      if (elapsedTicks < _delay_ticks)
      {
        SDL_Delay(_delay_ticks - elapsedTicks);
      }
    }
  }
  game_delegate->fini();
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(g_window);
  IMG_Quit();
  Mix_Quit();
  SDL_Quit();
}

void engine_stop()
{
  _is_running = SDL_FALSE;
}

void engine_set_frame_rate(Uint32 frame_rate)
{
  _frame_rate  = frame_rate;
  _delay_ticks = 1000 / _frame_rate;
}
