#include "engine.h"
#include <SDL2/SDL.h>

static SDL_bool      _is_running = SDL_FALSE;
static SDL_Window*   _window;
static SDL_Renderer* _renderer;
static Uint32        _delayTicks;
static Uint32        _frame_rate;

static SDL_bool init(const GameSetting* setting)
{
  _window = SDL_CreateWindow(setting->window_title,
                             SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED,
                             setting->window_width,
                             setting->window_height,
                             SDL_WINDOW_SHOWN);

  if (_window == NULL)
    return SDL_FALSE;

  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  if (_renderer == NULL)
    return SDL_FALSE;
  engine_set_frame_rate(setting->frame_rate);
  return SDL_TRUE;
}

void engine_run(GameDelegate* game_delegate, const GameSetting* setting)
{
  void (*loop_fn)(void*, SDL_Renderer*);
  void (*event_fn)(void*, const SDL_Event*);
  void*     user_data;
  Uint32    elapsedTicks, beginingTick;
  SDL_Event event;

  user_data = game_delegate->user_data;
  loop_fn = game_delegate->loop;
  event_fn = game_delegate->event;

  if (init(setting) && game_delegate->init(user_data))
  {
    _is_running = SDL_TRUE;

    while (_is_running)
    {
      beginingTick = SDL_GetTicks();
      while (SDL_PollEvent(&event))
      {
        event_fn(user_data, &event);
      }
      loop_fn(user_data, _renderer);
      elapsedTicks = SDL_GetTicks() - beginingTick;
      if (elapsedTicks < _delayTicks)
      {
        SDL_Delay(_delayTicks - elapsedTicks);
      }
    }
  }
  game_delegate->fini(user_data);
  SDL_DestroyRenderer(_renderer);
  SDL_DestroyWindow(_window);
  SDL_Quit();
}

void engine_stop() { _is_running = SDL_FALSE; }

void engine_set_frame_rate(Uint32 frame_rate)
{
  _frame_rate = frame_rate;
  _delayTicks = 1000 / _frame_rate;
}

SDL_Renderer* engine_get_renderer() { return _renderer; }
SDL_Window*   engine_get_window() { return _window; }
