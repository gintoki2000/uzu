#include "ui_msgbox_w_icon.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"

extern RENDERER* gRenderer;

#define ENTRY UI_MsgBoxWIcon_Entry
#define MAX_NENTRY 5
#define ENTRY_HEIGHT (16 + 4)
#define ENTRY_WIDTH (120)
#define POSITION_X ((WIN_WIDTH / 2) - (ENTRY_WIDTH / 2))
#define POSITION_Y (WIN_HEIGHT / 2)
#define GAP (2)
#define INITIAL_ENTRY_X 50

static const COLOR _entryColor = { 0x00, 0x00, 0x00, 0xd0 };
static const COLOR _textColor  = { 0xff, 0xff, 0xff, 0xff };

static ENTRY _entries[MAX_NENTRY];
static float _entryX[MAX_NENTRY];
static u16   _nentry;
static FONT* _font;
static BOOL  _visible;

static void process_input(SDL_UNUSED void* arg)
{
  if (button_just_pressed(BUTTON_INTERACT))
  {
    _visible = FALSE;
    input_pop_state();
  }
}

void ui_msgbox_w_icon_init(void)
{
  _font = get_font(FONT_DAMAGE_INDICATOR);
}

void ui_msgbox_w_icon_fini(void)
{
}

void ui_msgbox_w_icon_draw(void)
{
  if (!_visible)
    return;
  RECT icon_rect;
  RECT entry_rect;
  int  ry = POSITION_Y - ((ENTRY_HEIGHT + GAP) * _nentry) / 2;
  for (int i = 0; i < _nentry; ++i)
  {

    entry_rect.x = POSITION_X + _entryX[i];
    entry_rect.y = ry + i * (ENTRY_HEIGHT + GAP);
    entry_rect.w = ENTRY_WIDTH;
    entry_rect.h = ENTRY_HEIGHT;

    SDL_SetRenderDrawColor(gRenderer,
                           _entryColor.r,
                           _entryColor.g,
                           _entryColor.b,
                           _entryColor.a);

    SDL_RenderFillRect(gRenderer, &entry_rect);

    icon_rect.x = POSITION_X + _entryX[i];
    icon_rect.y = ry + i * (ENTRY_HEIGHT + GAP);
    icon_rect.w = 16;
    icon_rect.h = 16;
    SDL_RenderCopy(gRenderer,
                   get_texture(_entries[i].icon.textureId),
                   &_entries[i].icon.rect,
                   &icon_rect);

    FC_DrawColor(_font,
                 gRenderer,
                 icon_rect.x + 18,
                 icon_rect.y + 5,
                 _textColor,
                 _entries[i].text);
  }
}

void ui_msgbox_w_icon_update(void)
{
  if (!_visible)
    return;
  for (int i = 0; i < _nentry; ++i)
  {
    _entryX[i] += (0 - _entryX[i]) / 4;
  }
}

void ui_msgbox_w_icon_show(const ENTRY* entries, u16 _num_entries)
{
  _num_entries = min(_num_entries, 5);
  memcpy(_entries, entries, sizeof(ENTRY) * _num_entries);
  _visible = TRUE;
  _nentry  = _num_entries;
  for (int i = 0; i < _num_entries; ++i)
  {
    _entryX[i] = INITIAL_ENTRY_X + i * 10;
  }
  input_push_state(INPUT_STATE_INST_1(process_input));
}
#undef ENTRY
