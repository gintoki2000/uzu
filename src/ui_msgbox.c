#include "ui_msgbox.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "toolbox/toolbox.h"
#include "ui_helper.h"

static BOOL _visible;
static s32  _ticks;
static char _msg[512];

extern SDL_Renderer* g_renderer;

static const RECT      _box_rect         = { WIN_WIDTH / 2 - 50, WIN_HEIGHT / 2 - 15, 100, 30 };
static const RECT      _text_rect        = { WIN_WIDTH / 2 - 46, WIN_HEIGHT / 2 - 11, 94, 24 };
static const FC_Effect _text_effect      = { .alignment = FC_ALIGN_CENTER,
                                        .scale     = { 1.f, 1.f },
                                        .color     = { 0xff, 0xff, 0xff, 0xff } };
static const COLOR     _background_color = { 0x00, 0x00, 0x00, 0x90 };
static const COLOR     _border_color     = { 0xff, 0xff, 0xff, 0x80 };

static Callback _close_hook;

static void close()
{
  input_pop_state();
  _ticks   = -1;
  _visible = FALSE;
  INVOKE_EVENT(_close_hook, NULL);
  _close_hook = (Callback){ 0 };
}
static void process_input(SDL_UNUSED void* arg)
{
  if (_ticks < 0)
  {
    if (button_just_pressed(BUTTON_INTERACT) | button_just_pressed(BUTTON_CANCEL))
    {
      close();
    }
  }
}

void ui_msgbox_display(const char* msg)
{
  ui_msgbox_display_timed(msg, -1);
}

void ui_msgbox_display_timed(const char* msg, s32 ticks)
{
  SDL_strlcpy(_msg, msg, 511);
  _ticks   = ticks;
  _visible = TRUE;
  input_push_state(INPUT_STATE_INST_1(process_input));
}

void ui_msgbox_close_hook(Callback callback)
{
  _close_hook = callback;
}

void ui_msgbox_draw()
{
  if (!_visible)
    return;

  if (_ticks > 0 && !(--_ticks))
  {
    close();
    return;
  }

  draw_bordered_box(&_box_rect, _background_color, _border_color);
  FC_DrawBoxEffect(get_font(FONT_ITEM_PICKED_UP), g_renderer, _text_rect, _text_effect, _msg);
}
