#include "dungeon.h"
#include "SDL_image.h"
#include "behaviour_tree.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/engine.h"
#include "engine/keyboard.h"
#include "entity_factory.h"
#include "map.h"
#include "resources.h"
#include "scene.h"
#include "session.h"
#include "toolbox/toolbox.h"
#include <stdio.h>
#include <stdlib.h>

SDL_Rect g_viewport;

static const Scene* _curr_scr = NULL;

extern SDL_Renderer* g_renderer;
extern SDL_Window*   g_window;

Cursor g_cursor_cross   = { .sprite = { TEX_CURSOR_CROSS, { 0, 0, 9, 9 } }, .hotspot = { 4, 4 } };
Cursor g_cursor_pointer = { .sprite = { TEX_CURSOR_POINTER, { 0, 0, 9, 9 } }, .hotspot = { 0 } };

#define CURSOR_MAX_STATES 10
static Cursor _cursor_state[CURSOR_MAX_STATES];
static u8     _cursor_state_count = 0;

#define SHOW_FPS 1

#if SHOW_FPS
static FONT* _fps_font;
#endif

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

  SDL_ShowCursor(SDL_DISABLE);
  push_cursor_state(g_cursor_pointer);
  set_scene(&g_main_menu);

#if SHOW_FPS
  _fps_font = get_font(FONT_DAMAGE_INDICATOR);
#endif

  return TRUE;
}

static void on_game_fini()
{
  if (_curr_scr != NULL)
    _curr_scr->on_unload();
  resources_unload();
}
static void draw_cursor(void)
{
  SDL_Point mouse_position;
  SDL_Rect  dst;

  Cursor cursor = _cursor_state[_cursor_state_count - 1];

  SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
  mouse_position.x /= SCL_X;
  mouse_position.y /= SCL_Y;

  dst.x = mouse_position.x - cursor.hotspot.x;
  dst.y = mouse_position.y - cursor.hotspot.y;
  dst.w = cursor.sprite.rect.w;
  dst.h = cursor.sprite.rect.h;

  SDL_RenderCopy(g_renderer, get_texture(cursor.sprite.texture_id), &cursor.sprite.rect, &dst);
}
static void on_game_loop()
{
  u32 start = SDL_GetTicks();
  if (_curr_scr != NULL)
    _curr_scr->on_update();
#if SHOW_FPS
  FC_Draw(_fps_font, g_renderer, 0, 0, "%dms", SDL_GetTicks() - start);
#endif
  draw_cursor();
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

void push_cursor_state(Cursor cursor)
{
  if (_cursor_state_count < CURSOR_MAX_STATES)
    _cursor_state[_cursor_state_count++] = cursor;
}

void pop_cursor_state()
{
  if (_cursor_state_count)
    _cursor_state_count--;
}
