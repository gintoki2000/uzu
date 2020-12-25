#include "behaviour_tree.h"
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <toolbox/toolbox.h>

#include <components.h>
#include <constances.h>
#include <dungeon.h>
#include <entity_factory.h>
#include <generator.h>
#include <map.h>
#include <map_data.h>
#include <map_loader.h>
#include <resources.h>

#include <ecs/ecs.h>

#include <engine/engine.h>
#include <engine/keyboard.h>

SDL_Rect g_viewport;

static const Screen* _curr_scr = NULL;

extern const Screen  g_gamescr;
extern SDL_Renderer* g_renderer;

static void on_game_quit();
static BOOL on_game_init();
static void on_game_fini();
static void on_game_quit();
static void on_game_loop();
static void on_event(const SDL_Event* e);

static BOOL on_game_init()
{
  /*load asserts*/
  IMG_Init(IMG_INIT_PNG);
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
  {
    ERROR("open audio failed\n");
    return FALSE;
  }

  if (!resources_load())
  {
    ERROR("load resources failed\n");
    return FALSE;
  }

  /*set scale*/
  SDL_RenderSetScale(g_renderer, SCL_X, SCL_Y);

  g_viewport = (SDL_Rect){ 0, 0, WIN_WIDTH, WIN_HEIGHT };

  /*init keyboard*/
  keybroad_init();

  set_scr(&g_gamescr);

  return TRUE;
}

static void on_game_fini()
{
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  resources_unload();
  IMG_Quit();
  Mix_Quit();
}

static void on_game_loop()
{
  if (_curr_scr != NULL)
    _curr_scr->on_update();
}

static void on_event(const SDL_Event* e)
{
  if (e->type == SDL_QUIT)
  {
    on_game_quit();
  }
  else if (_curr_scr != NULL)
  {
    _curr_scr->on_event(e);
  }
}

static void on_game_quit()
{
  engine_stop();
}

void set_scr(const Screen* newscr)
{
  if (_curr_scr == newscr)
    return;
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  if (newscr != NULL)
    newscr->on_load();
  _curr_scr = newscr;
}

static GameDelegate delegate = {
  .init  = on_game_init,
  .fini  = on_game_fini,
  .loop  = on_game_loop,
  .event = on_event,
};

static GameSetting setting = {
  .frame_rate    = 50,
  .window_title  = "dungeon II",
  .window_width  = 480 * 2,
  .window_height = 320 * 2,
};

int main()
{
  engine_run(&delegate, &setting);
  return 0;
}
