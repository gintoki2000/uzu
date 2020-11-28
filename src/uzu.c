#include "SDL_render.h"
#include "ecs/common.h"
#include "toolbox/sprite.h"
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <engine/engine.h>
#include <ecs/world.h>
#include <common.h>
#include <components.h>

#include <system/draw_system.h>
#include <system/movement_system.h>
#include <system/player_controller_system.h>

#define WIN_WIDTH 480
#define WIN_HEIGHT 360

typedef struct App
{
  Ecs*         ecs;
  ecs_entity_t player;
  SDL_Texture* player_tex;
} App;


static void     on_game_quit(App* app);
static SDL_bool on_game_init(App* app);
static void     on_game_fini(App* app);
static void     on_game_quit(App* app);
static void     on_game_loop(App* app, SDL_Renderer* renderer);

static ecs_entity_t create_player(Ecs* ecs, SDL_Texture* tex)
{
  ecs_entity_t p;
  Visual* visual;
  Transform* transform;
  CommandInput* command_input;
  PlayerTag* player_tag;

  p = ecs_create(ecs);

  visual = ecs_add(ecs, p, VISUAL);
  sprite_init(&visual->sprite, tex);

  transform = ecs_add(ecs, p, TRANSFORM);
  transform->x = WIN_WIDTH / 2;
  transform->y = WIN_HEIGHT / 2;

  player_tag = ecs_add(ecs, p, PLAYER_TAG);

  command_input = ecs_add(ecs, p, COMMAND_INPUT);

  return p;
}

static SDL_bool on_game_init(App* app)
{
  /*load asserts*/
  IMG_Init(IMG_INIT_PNG);
  app->player_tex = IMG_LoadTexture(uzu_get_renderer(), "asserts/knight_f_idle_anim_f0.png");

  /*init ecs*/
  EcsType types[NUM_COMPONENTS] = {
    [VISUAL] = (EcsType) { .size = sizeof(Visual) },
    [TRANSFORM] = (EcsType) { .size = sizeof(Transform) },
    [ANIMATION] = (EcsType) { .size = sizeof(Animation) },
    [PLAYER_TAG] = (EcsType) { .size = sizeof(PlayerTag) },
    [COMMAND_INPUT] = (EcsType) { .size = sizeof(CommandInput) } 
  };

  app->ecs = ecs_new(types, NUM_COMPONENTS);

  app->player = create_player(app->ecs, app->player_tex);
  return SDL_TRUE;
}

static void on_game_fini(App* app) 
{
  ecs_del(app->ecs);
  SDL_DestroyTexture(app->player_tex);
  IMG_Quit();
}

static void on_game_loop(App* app, SDL_Renderer* renderer)
{
  SDL_RenderClear(renderer);
  PlayerControllerSystem(app->ecs);
  MovementSystem(app->ecs);
  DrawSystem(app->ecs, renderer);
  SDL_RenderPresent(renderer);
}


static void game_on_event(void* user_data, const SDL_Event* e)
{
  (void)user_data;

  // TODO: chuyển đổi SDL_event -> game game_on_event
  if (e->type == SDL_QUIT)
  {
    on_game_quit(user_data);
  }
}

static void on_game_quit(App* app)
{
  (void)app;
  uzu_stop();
}
/***********************************************************/
App app;

GameDelegate delegate = (GameDelegate){
  .init = (SDL_bool(*)(void*))on_game_init,
  .fini = (void(*)(void*))on_game_fini,
  .loop = (void(*)(void*, SDL_Renderer*))on_game_loop,
  .event = game_on_event,
  .user_data = &app,
};
GameSetting setting = (GameSetting){
  .frame_rate = 30,
  .window_title = "app",
  .window_width = 480,
  .window_height = 320
};

int main()
{
  uzu_run(&delegate, &setting);
  return 0;
}
