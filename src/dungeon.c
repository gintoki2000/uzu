#include "behaviour_tree.h"
#include <SDL2/SDL_image.h>

#include <stdio.h>
#include <stdlib.h>

#include <toolbox/toolbox.h>

#include <components.h>
#include <constances.h>
#include <dungeon.h>
#include <entity_factory.h>
#include <map.h>
#include <resources.h>
#include <scene.h>

#include <ecs/ecs.h>

#include <engine/engine.h>
#include <engine/keyboard.h>
#include "main_menu.h"
#include <session.h>

SDL_Rect g_viewport;

static const Scene* _curr_scr = NULL;

extern SDL_Renderer* g_renderer;
extern SDL_Window*   g_window;

static void on_game_quit();
static BOOL on_game_init();
static void on_game_fini();
static void on_game_quit();
static void on_game_loop();
static void on_event(const SDL_Event* e);

static BOOL on_game_init()
{
  SDL_RenderSetScale(g_renderer, SCL_X, SCL_Y);

  srand(SDL_GetTicks());

  g_viewport = (SDL_Rect){ 0, 0, WIN_WIDTH, WIN_HEIGHT };

  if (!resources_load())
  {
    ERROR("load resources failed\n");
    return FALSE;
  }

  new_game(JOB_KNIGHT);
  set_scene(&g_game_scene);

  return TRUE;
}

static void on_game_fini()
{
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  resources_unload();
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

void set_scene(const Scene* newscr)
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
  .frame_rate    = 60,
  .window_title  = "dungeon II",
  .window_width  = WIN_WIDTH * SCL_X,
  .window_height = WIN_HEIGHT * SCL_Y,
};

int main()
{
  engine_run(&delegate, &setting);
  return 0;
}
