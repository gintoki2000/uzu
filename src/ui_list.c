#include "ui_list.h"
#include "SDL_mixer.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"

#define UI_LIST_WIDTH 100
#define UI_LIST_MAX_ITEMS 10

static char*    _items[UI_LIST_MAX_ITEMS];
static s32      _cnt;
static Callback _callback[UI_LIST_NUM_EVENTS];
static BOOL     _visible;
static int      _pos_x;
static int      _pos_y;
static int      _selected;

extern SDL_Renderer* g_renderer;

static void process_key_input()
{
  if (key_just_pressed(KEY_UP))
  {
    _selected--;
    if (_selected < 0)
      _selected = _cnt - 1;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_DOWN))
  {
    _selected++;
    if (_selected > _cnt - 1)
      _selected = 0;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_B))
  {
    ui_list_close();
  }

  if (key_just_pressed(KEY_A))
  {
    ui_list_close();

    if (_callback[UI_LIST_ON_SELECT].func != NULL)
      INVOKE_CALLBACK(_callback[UI_LIST_ON_SELECT], void, _items[_selected]);
  }
}

void ui_list_display(const char** items, u32 cnt)
{
  _visible = TRUE;

  for (int i = 0; i < _cnt; ++i)
  {
    free(_items[i]);
    _items[i] = NULL;
  }

  _cnt = min(cnt, UI_LIST_MAX_ITEMS);

  for (s32 i = 0; i < _cnt; ++i)
  {
    _items[i] = strdup(items[i]);
  }

  _selected = 0;

  keybroad_push_state(process_key_input);
  Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
}

void ui_list_draw()
{
  if (!_visible)
    return;

  draw_box_w_border(&(RECT){ _pos_x, _pos_y, UI_LIST_WIDTH, _cnt * 20 },
                    UI_COLOR_BG,
                    UI_COLOR_BORDER);

  COLOR color;
  for (int i = 0; i < _cnt; ++i)
  {
    color = i != _selected ? UI_COLOR_TEXT : UI_COLOR_TEXT_SELECT;

    FC_DrawEffect(get_font(FONT_ITEM_PICKED_UP),
                  g_renderer,
                  _pos_x + UI_LIST_WIDTH / 2,
                  _pos_y + i * 20 + 7,
                  (FC_Effect){
                      .color     = color,
                      .scale     = (FC_Scale){ 1.f, 1.f },
                      .alignment = FC_ALIGN_CENTER,
                  },
                  _items[i]);
  }
}

void ui_list_close()
{
  if (_visible)
  {
    keybroad_pop_state();
    _visible = FALSE;

    if (_callback[UI_LIST_ON_CLOSE].func != NULL)
      INVOKE_CALLBACK_NOARGS(_callback[UI_LIST_ON_CLOSE], void);

    Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
  }
}

void ui_list_hook(u32 event_id, Callback callback)
{
  ASSERT(event_id < UI_LIST_NUM_EVENTS && "invalid event_id");
  _callback[event_id] = callback;
}

void ui_list_set_pos(s32 x, s32 y)
{
  _pos_x = x;
  _pos_y = y;
}
