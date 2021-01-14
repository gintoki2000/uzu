#include "main_menu.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"

static void on_load();
static void on_unload();
static void on_update();
static void on_event(const SDL_Event*);
static void process_key_input(void);

Scene g_main_menu = {
  .on_load   = on_load,
  .on_unload = on_unload,
  .on_update = on_update,
  .on_event  = on_event,
};

#define MAIN_MENU_POS_X 117
#define MAIN_MENU_POS_Y 78
#define MAIN_MENU_NUM_OPS 2

static TEXTURE*   _texture;
static TEXTURE*   _pointer;
static const RECT _dst             = { 117, 78, 89, 85 };
static const char _text_new_game[] = "New Game";
static const char _text_exit[]     = "Exit";
static s8         _select;

static const char* const _options[] = { _text_new_game, _text_exit, NULL };

extern RENDERER* g_renderer;

static void on_load()
{
  _texture = get_texture(TEX_UI_MAIN_MENU);
  _pointer = get_texture(TEX_UI_MAIN_MENU_POINTER);
  keybroad_push_state(process_key_input);
  _select = 0;
}

static void on_unload()
{
}

static void on_update()
{
  SDL_RenderCopy(g_renderer, _texture, NULL, &_dst);

  FC_Effect effect =
      (FC_Effect){ .color = COLOR_WHITE, .alignment = FC_ALIGN_CENTER, .scale = { 1.f, 1.f } };

  for (int i = 0; _options[i] != NULL; ++i)
  {
    FC_DrawEffect(get_font(FONT_DAMAGE_INDICATOR),
                  g_renderer,
                  MAIN_MENU_POS_X + 44,
                  MAIN_MENU_POS_Y + 10 + i * 15,
                  effect,
                  "%s",
                  _options[i]);
  }
}

static void on_event(const SDL_Event* e)
{
  (void)e;
}
static void process_key_input(void)
{
}
