#include "ui_list.h"
#include "SDL_mixer.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"
#include "dungeon.h"

#define UI_LIST_WIDTH 100
#define MAX_ENTRIES 10
#define ENTRY_WIDTH 100
#define ENTRY_HEIGHT 20
#define ENTRY_VGAP 2
#define FIRST_ENTRY_Y 7

static UI_List_Entry _entries[MAX_ENTRIES];
static s32           _num_entries;
static Callback      _events[UI_LIST_NUM_EVENTS];
static BOOL          _visible;
static int           _pos_x;
static int           _pos_y;
static int           _selected;

extern SDL_Renderer* g_renderer;

static int get_height()
{
  return _num_entries * ENTRY_HEIGHT + (_num_entries - 1) * ENTRY_VGAP;
}

static RECT get_bound(void)
{
  RECT bound;
  bound.x = _pos_x;
  bound.y = _pos_y;
  bound.w = UI_LIST_WIDTH;
  bound.h = get_height();
  return bound;
}

static void process_key_input(SDL_UNUSED void* arg)
{
  SDL_Point mouse_position;
  SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
  mouse_position.x /= SCL_X;
  mouse_position.y /= SCL_Y;

  RECT bound         = get_bound();
  BOOL mouse_in_rect = SDL_PointInRect(&mouse_position, &bound);

  if (mouse_in_rect)
  {
    int y = mouse_position.y - _pos_y - FIRST_ENTRY_Y;
    y /= (ENTRY_HEIGHT + ENTRY_VGAP);
    _selected = y;
  }

  if (button_just_pressed(BUTTON_UP))
  {
    _selected = max(0, _selected - 1);
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (button_just_pressed(BUTTON_DOWN))
  {
    _selected = min(MAX_ENTRIES - 1, _selected + 1);
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (button_just_pressed(BUTTON_CANCEL))
  {
    ui_list_close();
  }

  if (button_just_pressed(BUTTON_INTERACT) ||
      (mouse_in_rect && mouse_button_just_pressed(SDL_BUTTON_LEFT)))
  {
    ui_list_close();
    INVOKE_EVENT(_events[UI_LIST_ON_SELECT], (const char*)_entries[_selected].text);
  }
}

void ui_list_display(const char** items, u32 cnt)
{
  _visible     = TRUE;
  _num_entries = min(cnt, MAX_ENTRIES);

  for (s32 i = 0; i < _num_entries; ++i)
  {
    SDL_strlcpy(_entries[i].text, items[i], UI_LIST_ENTRY_MAX_LEN);
  }
  _selected = 0;

  extern Cursor g_cursor_pointer;

  input_push_state(INPUT_STATE_INST_1(process_key_input));
  push_cursor_state(g_cursor_pointer);
  Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
}

void ui_list_draw()
{
  if (!_visible)
    return;

  draw_bordered_box(&(RECT){ _pos_x, _pos_y, UI_LIST_WIDTH, _num_entries * 20 },
                    UI_COLOR_BG,
                    UI_COLOR_BORDER);

  COLOR color;
  for (int i = 0; i < _num_entries; ++i)
  {
    color = (i != _selected) ? UI_COLOR_TEXT : UI_COLOR_TEXT_SELECT;
    FC_DrawEffect(get_font(FONT_ITEM_PICKED_UP),
                  g_renderer,
                  _pos_x + UI_LIST_WIDTH / 2,
                  _pos_y + i * (ENTRY_HEIGHT + ENTRY_VGAP) + FIRST_ENTRY_Y,
                  (FC_Effect){
                      .color     = color,
                      .scale     = (FC_Scale){ 1.f, 1.f },
                      .alignment = FC_ALIGN_CENTER,
                  },
                  _entries[i].text);
  }
}

void ui_list_close()
{
  if (_visible)
  {
    input_pop_state();
    pop_cursor_state();
    _visible = FALSE;
    Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
  }
}

void ui_list_hook(u32 event_id, Callback callback)
{
  ASSERT(event_id < UI_LIST_NUM_EVENTS && "invalid event_id");
  _events[event_id] = callback;
}

void ui_list_set_pos(s32 x, s32 y)
{
  _pos_x = (x == UI_LIST_POS_CENTER_X) ? WIN_WIDTH / 2 - UI_LIST_WIDTH / 2 : x;
  _pos_y = (y == UI_LIST_POS_CENTER_Y) ? WIN_HEIGHT / 2 - get_height() / 2 : y;
}
