#include "ui_msgbox.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"
#include <toolbox/toolbox.h>

static BOOL       _visible;
static s32        _ticks;
static const char _msg[512];

extern SDL_Renderer* g_renderer;

const static RECT      BOX_RECT    = { WIN_WIDTH / 2 - 50, WIN_HEIGHT / 2 - 15, 100, 30 };
const static RECT      TEXT_RECT   = { WIN_WIDTH / 2 - 46, WIN_HEIGHT / 2 - 11, 94, 24 };
const static FC_Effect TEXT_EFFECT = { .alignment = FC_ALIGN_CENTER,
                                       .scale     = { 1.f, 1.f },
                                       .color     = { 0xff, 0xff, 0xff, 0xff } };

static void close()
{
  keybroad_pop_state();
  _ticks   = -1;
  _visible = FALSE;
}
static void process_key_input(void)
{
  if (_ticks < 0)
  {
    if (key_just_pressed(KEY_A))
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
  keybroad_push_state(process_key_input);
}

void ui_msgbox_draw()
{
  if (!_visible)
    return;

  if (_ticks > 0 && !(--_ticks))
  {
    close();
  }

  draw_box_w_border(&BOX_RECT, COLOR_BLACK, COLOR_WHITE);
  FC_DrawBoxEffect(get_font(FONT_ITEM_PICKED_UP), g_renderer, TEXT_RECT, TEXT_EFFECT, _msg);
}
