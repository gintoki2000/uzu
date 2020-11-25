#include "engine.h"
#include "SDL_events.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"
#include "SDL_timer.h"
#include "SDL_video.h"

static SDL_bool _is_running = SDL_FALSE;
static SDL_Window *_window;
static SDL_Renderer *_renderer;
static Uint32 _delayTicks;
static Uint32 _frame_rate;

static SDL_bool init(const GameSetting *game_setting) {
  _window = SDL_CreateWindow(game_setting->window_title, SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, game_setting->window_width,
                             game_setting->window_height, SDL_WINDOW_SHOWN);

  if (_window == NULL)
    return SDL_FALSE;
  
  _renderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED);
  if (_renderer == NULL)
    return SDL_FALSE;
  uzu_set_frame_rate(game_setting->frame_rate);
  return SDL_TRUE;
}

void uzu_run(GameDelegate *game_delegate, const GameSetting *game_setting) {
  void (*loop_fn)(void *, SDL_Renderer *);
  void (*event_fn)(void *, const SDL_Event*);
  void *user_data;
  Uint32 elapsedTicks, beginingTick;
  SDL_Event event;

  user_data = game_delegate->user_data;
  loop_fn = game_delegate->loop;
  event_fn = game_delegate->event;

  if (init(game_setting) && game_delegate->init(user_data)) {
    _is_running = SDL_TRUE;

    while (_is_running) {
      beginingTick = SDL_GetTicks();
      while(SDL_PollEvent(&event))
      {
        event_fn(user_data, &event); 
      }
      loop_fn(user_data, _renderer);
      elapsedTicks = SDL_GetTicks() - beginingTick;
      if (elapsedTicks < _delayTicks) {
        SDL_Delay(_delayTicks - elapsedTicks);
      }
    }
  }
  game_delegate->fini(game_delegate->user_data);
}

void uzu_stop() { _is_running = SDL_FALSE; }

void uzu_set_frame_rate(Uint32 frame_rate) {
  _frame_rate = frame_rate;
  _delayTicks = 1000 / _frame_rate;
}
