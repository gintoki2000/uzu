#include "ui_quality.h"
#include "SDL.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"
extern SDL_Renderer* g_renderer;

static char* _title;
static BOOL  _visible;
static u32   _value;
static u32   _maxValue;
static u32   _minValue;

static Callback _callbacks[UI_QUALITY_NUM_EVENTS];

#define UI_QUALITY_X 50
#define UI_QUALITY_Y 50

static const RECT  _rectBox     = { UI_QUALITY_X, UI_QUALITY_Y, 50, 30 };
static const RECT  _rectTile    = { UI_QUALITY_X + 3, UI_QUALITY_Y + 3, 44, 24 };
static const RECT  _rectQuality = { UI_QUALITY_X, UI_QUALITY_Y + 12, 44, 24 };
static const COLOR _colorBg     = { 0x00, 0x00, 0x00, 0x90 };
static const COLOR _colorBorder = { 0xff, 0xff, 0xff, 0x80 };

static void process_key_input()
{
  if (button_just_pressed(BUTTON_INTERACT))
  {
    input_pop_state();
    _visible = FALSE;
    INVOKE_EVENT(_callbacks[UI_QUALITY_ON_SELECTED], (u32)_value);
    return;
  }

  if (button_just_pressed(BUTTON_CANCEL))
  {

    input_pop_state();
    _visible = FALSE;
    return;
  }

  if (button_just_pressed(BUTTON_RIGHT))
  {
    if (_value < _maxValue)
    {
      ++_value;
    }
    else
    {
      // TODO: play sound
    }
  }

  if (button_just_pressed(BUTTON_LEFT))
  {
    if (_value > _minValue)
    {
      --_value;
    }
    else
    {
      // TODO: play sound
    }
  }
}

void ui_quality_display(const char* title, u32 initalValue, u32 minValue, u32 maxValue)
{
  _visible = TRUE;
  _title   = SDL_strdup(title);
  _value     = initalValue;
  _maxValue  = maxValue;
  _minValue  = minValue;
  input_push_state(INPUT_STATE_INST_1(process_key_input));
}

void ui_quality_draw()
{
  if (!_visible)
    return;

  FONT*     font   = get_font(FONT_DAMAGE_INDICATOR);
  FC_Effect effect = { .scale = { 1.f, 1.f }, .alignment = FC_ALIGN_CENTER, .color = gColorWhite };

  draw_bordered_box(&_rectBox, _colorBg, _colorBorder);

  FC_DrawBoxColor(font, g_renderer, _rectTile, gColorWhite, _title);
  FC_DrawBoxEffect(font, g_renderer, _rectQuality, effect, "< %u >", _value);
}

void ui_quality_hook(s32 event_id, Callback callback)
{
  _callbacks[event_id] = callback;
}
