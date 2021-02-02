#include "main_menu.h"
#include "constances.h"
#include "dungeon.h"
#include "engine/engine.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "session.h"

static void on_load();
static void on_unload();
static void on_update();
static void on_event(const SDL_Event*);
static void process_key_input(void);

const Scene g_main_menu = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_update = on_update,
  .on_event  = on_event,
};

#define MAIN_MENU_POS_X 117
#define MAIN_MENU_POS_Y 78
#define MAIN_MENU_NUM_OPS 3

static TEXTURE*   _texture;
static TEXTURE*   _pointer;
static FONT*      _font;
static const RECT _dst             = { 117, 78, 89, 85 };
static const char _text_new_game[] = "New Game";
static const char _text_exit[]     = "Exit";
static const char _text_about[]    = "About";
static const char _text_load[]     = "Load";
static s8         _select;

static const char* const _options[MAIN_MENU_NUM_OPS] = { _text_new_game, _text_about, _text_exit };

extern RENDERER* g_renderer;

static void on_load()
{
  _texture = get_texture(TEX_UI_MAIN_MENU);
  _pointer = get_texture(TEX_UI_MAIN_MENU_POINTER);
  _font    = get_font(FONT_DAMAGE_INDICATOR);
  _select  = 0;
  keybroad_push_state(process_key_input);
}

static void on_unload()
{
}

static void on_update()
{
  int       x, y;
  FC_Effect effect;

  SDL_RenderCopy(g_renderer, _texture, NULL, &_dst);

  effect = (FC_Effect){
    .color     = COLOR_WHITE,
    .alignment = FC_ALIGN_CENTER,
    .scale     = { 1.f, 1.f },
  };

  x = MAIN_MENU_POS_X + 44;
  for (int i = 0; i < MAIN_MENU_NUM_OPS; ++i)
  {
    y = MAIN_MENU_POS_Y + 10 + i * 15;
    FC_DrawEffect(_font, g_renderer, x, y, effect, "%s", _options[i]);
  }
  x = MAIN_MENU_POS_X + 10;
  y = MAIN_MENU_POS_Y + 11 + _select * 15;
  SDL_RenderCopy(g_renderer, _pointer, NULL, &(RECT){ x, y, 5, 5 });
}

static void on_event(const SDL_UNUSED SDL_Event* e)
{
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_DOWN) && _select < MAIN_MENU_NUM_OPS - 1)
  {
    ++_select;
  }

  if (key_just_pressed(KEY_UP) && _select > 0)
  {
    --_select;
  }

  if (key_just_pressed(KEY_A))
  {
    if (_options[_select] == _text_new_game)
    {
      keybroad_pop_state();
      new_game(JOB_WIZZARD);
      set_scene(&g_game_scene);
    }
    else if (_options[_select] == _text_exit)
    {
      engine_stop();
    }
  }
}
