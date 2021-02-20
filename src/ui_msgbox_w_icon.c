#include "ui_msgbox_w_icon.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "constances.h"

extern RENDERER* g_renderer;

#define ENTRY UI_MsgBoxWIcon_Entry
#define MAX_NENTRY 5
#define ENTRY_HEIGHT (16 + 4)
#define ENTRY_WIDTH (120)
#define POSITION_X ((WIN_WIDTH / 2) - (ENTRY_WIDTH / 2))
#define POSITION_Y (WIN_HEIGHT / 2)
#define GAP (2)

static const COLOR _entry_color = { 0x00, 0x00, 0x00, 0xd0 };
static const COLOR _text_color  = { 0xff, 0xff, 0xff, 0xff };

static ENTRY _entries[MAX_NENTRY];
static u16   _nentry;
static FONT* _font;
static BOOL  _visible;

static void process_input(void)
{
  if (key_just_pressed(KEY_A))
  {
    _visible = FALSE;
    keybroad_pop_state();
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
  int ry = POSITION_Y - ((ENTRY_HEIGHT + GAP) * _nentry) / 2;
  for (int i = 0; i < _nentry; ++i)
  {

    entry_rect.x = POSITION_X;
    entry_rect.y = ry + i * (ENTRY_HEIGHT + GAP);
    entry_rect.w = ENTRY_WIDTH;
    entry_rect.h = ENTRY_HEIGHT;

    SDL_SetRenderDrawColor(g_renderer,
                           _entry_color.r,
                           _entry_color.g,
                           _entry_color.b,
                           _entry_color.a);

    SDL_RenderFillRect(g_renderer, &entry_rect);

    icon_rect.x = POSITION_X;
    icon_rect.y = ry + i * (ENTRY_HEIGHT + GAP);
    icon_rect.w = 16;
    icon_rect.h = 16;
    SDL_RenderCopy(g_renderer,
                   get_texture(_entries[i].icon.texture_id),
                   &_entries[i].icon.rect,
                   &icon_rect);

    FC_DrawColor(_font, g_renderer, icon_rect.x + 18, icon_rect.y + 5, _text_color, _entries[i].text);
  }
}

void ui_msgbox_w_icon_update(void)
{
}

void ui_msgbox_w_icon_show(const ENTRY* entries, u16 nentry)
{
  nentry = min(nentry, 5);
  memcpy(_entries, entries, sizeof(ENTRY) * nentry);
  _visible = TRUE;
  _nentry = nentry;
  keybroad_push_state(process_input);
}
#undef ENTRY
