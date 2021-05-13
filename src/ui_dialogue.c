#include "ui_dialogue.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "global.h"
#include "resources.h"
#include "ui_list.h"

#define MAX_TEXT_LENGTH 255

static const char* _text;
static FONT*       _font;
static TEXTURE*    _texture_dialogue;
static u16         _text_length;
static char        _displayed_text[MAX_TEXT_LENGTH + 1];
static u16         _displayed_text_length;
static u16         _delay;        // thời gian cho kí tự tiếp theo xuất hiện
static u16         _elpased_time; // thời gian kể từ khi kí tự trước đó xuất hiện
static BOOL        _is_visible;

static const RECT _rect_dialogue = { 10, 167, 299, 60 };
static const RECT _rect_textbox  = { 10 + 17, 167 + 5, 282, 54 };

extern RENDERER* g_renderer;

void ui_dialogue_init(void)
{
  _font                  = get_font(FONT_DAMAGE_INDICATOR);
  _texture_dialogue      = get_texture(TEX_UI_DIALOGUE);
  _is_visible            = FALSE;
  _delay                 = 3;
  _displayed_text[0]     = '\0';
  _displayed_text_length = 0;
}

void ui_dialogue_fini()
{
}

void ui_dialogue_set_visible(SDL_bool value)
{
  _is_visible = value;
}

void ui_dialogue_set_delay(u16 value)
{
  _delay = value;
}

void ui_dialogue_set_text(const char* text)
{
  _text                  = text;
  _elpased_time          = 0;
  _displayed_text[0]     = '\0';
  _displayed_text_length = 0;
  _text_length           = SDL_strlen(text);
}

void ui_dialogue_update(void)
{
  if (!_is_visible)
    return;
  if (++_elpased_time >= _delay)
  {
    _elpased_time = 0;
    if (_displayed_text_length < MAX_TEXT_LENGTH && _displayed_text_length < _text_length)
    {
      _displayed_text[_displayed_text_length]     = _text[_displayed_text_length];
      _displayed_text[_displayed_text_length + 1] = '\0';
      ++_displayed_text_length;
    }
  }
}

void ui_dialogue_draw(void)
{
  if (!_is_visible)
    return;
  SDL_RenderCopy(g_renderer, _texture_dialogue, NULL, &_rect_dialogue);
  FC_DrawBoxColor(_font, g_renderer, _rect_textbox, gColorBlack, "%s", _displayed_text);
}
