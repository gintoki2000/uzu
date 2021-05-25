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
static TEXTURE*    _textureDialogue;
static u32         _textLength;
static char        _displayedText[MAX_TEXT_LENGTH + 1];
static u32         _displayedTextLength;
static u16         _delay;        // thời gian cho kí tự tiếp theo xuất hiện
static u32         _elpasedTime; // thời gian kể từ khi kí tự trước đó xuất hiện
static BOOL        _isVisible;

static const RECT _rectDialogue = { 10, 167, 299, 60 };
static const RECT _rectTextbox  = { 10 + 17, 167 + 5, 282, 54 };

extern RENDERER* gRenderer;

void ui_dialogue_init(void)
{
  _font                  = get_font(FONT_DAMAGE_INDICATOR);
  _textureDialogue      = get_texture(TEX_UI_DIALOGUE);
  _isVisible            = FALSE;
  _delay                 = 3;
  _displayedText[0]     = '\0';
  _displayedTextLength = 0;
}

void ui_dialogue_fini()
{
}

void ui_dialogue_set_visible(SDL_bool value)
{
  _isVisible = value;
}

void ui_dialogue_set_delay(u16 value)
{
  _delay = value;
}

void ui_dialogue_set_text(const char* text)
{
  _text                  = text;
  _elpasedTime          = 0;
  _displayedText[0]     = '\0';
  _displayedTextLength = 0;
  _textLength           = SDL_strlen(text);
}

void ui_dialogue_update(void)
{
  if (!_isVisible)
    return;
  if (++_elpasedTime >= _delay)
  {
    _elpasedTime = 0;
    if (_displayedTextLength < MAX_TEXT_LENGTH && _displayedTextLength < _textLength)
    {
      _displayedText[_displayedTextLength]     = _text[_displayedTextLength];
      _displayedText[_displayedTextLength + 1] = '\0';
      ++_displayedTextLength;
    }
  }
}

void ui_dialogue_draw(void)
{
  if (!_isVisible)
    return;
  SDL_RenderCopy(gRenderer, _textureDialogue, NULL, &_rectDialogue);
  FC_DrawBoxColor(_font, gRenderer, _rectTextbox, gColorBlack, "%s", _displayedText);
}
