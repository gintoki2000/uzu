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

const static RECT      BOX_RECT     = { WIN_WIDTH / 2 - 50, WIN_HEIGHT / 2 - 15, 100, 30 };
const static RECT      TEXT_RECT    = { WIN_WIDTH / 2 - 46, WIN_HEIGHT / 2 - 11, 94, 24 };
const static FC_Effect TEXT_EFFECT  = { .alignment = FC_ALIGN_CENTER,
                                       .scale     = { 1.f, 1.f },
                                       .color     = { 0xff, 0xff, 0xff, 0xff } };
const static COLOR     BG_COLOR     = { 0x00, 0x00, 0x00, 0x90 };
const static COLOR     BORDER_COLOR = { 0xff, 0xff, 0xff, 0x80 };

static void close()
{
  input_pop_state();
  _ticks   = -1;
  _visible = FALSE;
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
  strcpy(_msg, msg);
  _ticks   = ticks;
  _visible = TRUE;
  input_push_state(INPUT_STATE_INST_1(process_input));
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

  draw_bordered_box(&BOX_RECT, BG_COLOR, BORDER_COLOR);
  FC_DrawBoxEffect(get_font(FONT_ITEM_PICKED_UP), g_renderer, TEXT_RECT, TEXT_EFFECT, _msg);
}
