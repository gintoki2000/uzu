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
static void main_state_process_input(void);
static void draw_main_menu(void);
static void draw_about_panel(void);
static void about_state_process_input(void);

const Scene g_main_menu = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_update = on_update,
  .on_event  = on_event,
};

#define MAIN_MENU_POS_X 117
#define MAIN_MENU_POS_Y 78
#define MAIN_MENU_NUM_OPS 4

enum
{
  TITLE_SCENE_STATE_MAIN,
  TITLE_SCENE_STATE_ABOUT,
};

static TEXTURE*   _table;
static TEXTURE*   _pointer;
static TEXTURE*   _bg;
static FONT*      _font;
static const RECT _dst             = { 117, 78, 89, 85 };
static const char _text_new_game[] = "New Game";
static const char _text_exit[]     = "Exit";
static const char _text_about[]    = "About";
static const char _text_load[]     = "Load";
static s8         _select;
static u8         _state;

static const char* const _options[MAIN_MENU_NUM_OPS] = { _text_load,
                                                         _text_new_game,
                                                         _text_about,
                                                         _text_exit };

static const RECT _about_panel_rect = { 54, 40, 212, 160 };

static const char _about_content[] = "PHOENIX TEAM\n"
                                     "Dev: Bap\n"
                                     "Design: Hen, Cong\n"
                                     "Art: 0x75, Lam Giang";

static COLOR _about_panel_color = { 0x00, 0x00, 0x00, 0xd0 };

static FC_Effect _about_content_text_effect = {
  .alignment = FC_ALIGN_CENTER,
  .scale     = { 1.f, 1.f },
  .color     = { 0xff, 0xff, 0xff, 0xdf },
};

extern RENDERER* g_renderer;

static void on_load()
{
  _table   = get_texture(TEX_UI_MAIN_MENU);
  _pointer = get_texture(TEX_UI_MAIN_MENU_POINTER);
  _font    = get_font(FONT_DAMAGE_INDICATOR);
  _bg      = get_texture(TEX_TILESCR_BG);
  keybroad_push_state(main_state_process_input);
  Mix_PlayMusic(get_bg_mus(BG_MUS_TILE_SCREEN), -1);
}

static void on_unload()
{
  keybroad_pop_state();
}

static void on_update()
{
  switch (_state)
  {
  case TITLE_SCENE_STATE_MAIN:
    draw_main_menu();
    break;
  case TITLE_SCENE_STATE_ABOUT:
    draw_about_panel();
    break;
  }
}

static void on_event(const SDL_UNUSED SDL_Event* e)
{
}

static void main_state_process_input()
{
  if (key_just_pressed(KEY_DOWN) && _select < MAIN_MENU_NUM_OPS - 1)
  {
    ++_select;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_UP) && _select > 0)
  {
    --_select;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_A))
  {
    if (_options[_select] == _text_new_game)
    {
      set_scene(&g_select_character_scene);
    }
    else if (_options[_select] == _text_exit)
    {
      engine_stop();
    }
    else if (_options[_select] == _text_about)
    {
      _state = TITLE_SCENE_STATE_ABOUT;
      keybroad_push_state(about_state_process_input);
      Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
    }
  }
}
static void draw_main_menu(void)
{
  int       x, y;
  FC_Effect effect;

  SDL_RenderCopy(g_renderer, _bg, NULL, NULL);
  SDL_RenderCopy(g_renderer, _table, NULL, &_dst);

  effect = (FC_Effect){
    .color     = COLOR_WHITE,
    .alignment = FC_ALIGN_CENTER,
    .scale     = { 1.f, 1.f },
  };

  x = MAIN_MENU_POS_X + 44;
  for (int i = 0; i < MAIN_MENU_NUM_OPS; ++i)
  {
    y = MAIN_MENU_POS_Y + 10 + i * 15;
    FC_DrawEffect(_font, g_renderer, x, y, effect, _options[i]);
  }
  x = MAIN_MENU_POS_X + 10;
  y = MAIN_MENU_POS_Y + 11 + _select * 15;
  SDL_RenderCopy(g_renderer, _pointer, NULL, &(RECT){ x, y, 5, 5 });
}
static void draw_about_panel(void)
{
  SDL_RenderCopy(g_renderer, _bg, NULL, NULL);
  SDL_SetRenderDrawColor(g_renderer,
                         _about_panel_color.r,
                         _about_panel_color.g,
                         _about_panel_color.b,
                         _about_panel_color.a);
  SDL_RenderFillRect(g_renderer, &_about_panel_rect);
  FC_DrawBoxEffect(_font,
                   g_renderer,
                   _about_panel_rect,
                   _about_content_text_effect,
                   _about_content);
}

static void about_state_process_input()
{
  if (key_just_pressed(KEY_B))
  {
    _state = TITLE_SCENE_STATE_MAIN;
    keybroad_pop_state();
    Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
  }
}
