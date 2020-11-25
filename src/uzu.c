#include <engine/engine.h>
#include <stdio.h>

static void on_game_quit(void* user_data);
static SDL_bool on_game_init(void* user_data);

static SDL_bool on_game_init(void *user_data) {
  (void)user_data;
  return SDL_TRUE;
}

static void on_game_fini(void *user_data) { (void)user_data; }

static void loop(void *user_data, SDL_Renderer *renderer) {
  (void)user_data;
  (void)renderer;
}

static void event(void *user_data, const SDL_Event *e) {
  (void)user_data;

  if (e->type == SDL_QUIT) {
    on_game_quit( user_data );
  }
}

static void on_game_quit(void* user_data)
{
  (void)user_data;
  uzu_stop();
}

int main() {
  GameDelegate delegate = (GameDelegate){
      .init = on_game_init,
      .fini = on_game_fini,
      .loop = loop,
      .event = event,
      .user_data = NULL,
  };
  GameSetting setting = (GameSetting){.frame_rate = 30,
                                      .window_title = "uzu",
                                      .window_width = 480,
                                      .window_height = 320};
  uzu_run(&delegate, &setting);
  return 0;
}
