#include "ui_list.h"
#include "SDL_mixer.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "ui_helper.h"

#define UI_LIST_WIDTH 100

static char     _items[5][10];
static s32      _cnt;
static Callback _callback[UI_LIST_NUM_EVENTS];

extern SDL_Renderer* g_renderer;

BOOL     g_ui_list_visible;
int      g_ui_list_pos_x;
int      g_ui_list_pos_y;
Callback g_ui_list_on_sellect_cb;
int      g_ui_list_curr_idx;

typedef void (*ui_list_on_close_fn_t)(void*);
typedef void (*ui_list_on_select_fn_t)(void*, const char*, int);

static void process_key_input()
{
  if (key_just_pressed(KEY_UP))
  {
    g_ui_list_curr_idx--;
    if (g_ui_list_curr_idx < 0)
      g_ui_list_curr_idx = _cnt - 1;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_DOWN))
  {
    g_ui_list_curr_idx++;
    if (g_ui_list_curr_idx > _cnt - 1)
      g_ui_list_curr_idx = 0;
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }

  if (key_just_pressed(KEY_B))
  {
    ui_list_close();
  }

  if (key_just_pressed(KEY_A))
  {
    ui_list_close();
    ui_list_on_select_fn_t callback_fn = (ui_list_on_select_fn_t)_callback[UI_LIST_ON_SELECT].func;
    callback_fn(_callback[UI_LIST_ON_SELECT].user_data,
                _items[g_ui_list_curr_idx],
                g_ui_list_curr_idx);
  }
}

void ui_list_display(const char** items, u32 cnt)
{
  g_ui_list_visible = TRUE;

  _cnt = min(cnt, 5);

  for (u32 i = 0; i < cnt && i < 5; ++i)
  {
    strcpy(_items[i], items[i]);
  }

  g_ui_list_curr_idx = 0;

  keybroad_push_state(process_key_input);
  Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
}

void ui_list_draw()
{
  if (!g_ui_list_visible)
    return;

  draw_box_w_border(&(RECT){ g_ui_list_pos_x, g_ui_list_pos_y, UI_LIST_WIDTH, _cnt * 20 },
                    (COLOR){ 0, 0, 0, 0 },
                    (COLOR){ 255, 255, 255, 255 });

  COLOR color;
  for (int i = 0; i < _cnt; ++i)
  {
    color = i != g_ui_list_curr_idx ? (COLOR){ 255, 255, 255, 255 } : (COLOR){ 247, 161, 0, 255 };

    FC_DrawEffect(get_font(FONT_ITEM_PICKED_UP),
                  g_renderer,
                  g_ui_list_pos_x + UI_LIST_WIDTH / 2,
                  g_ui_list_pos_y + i * 20 + 7,
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
  if (g_ui_list_visible)
  {
    keybroad_pop_state();
    g_ui_list_visible = FALSE;

    ui_list_on_close_fn_t callback_fn = (ui_list_on_close_fn_t)_callback[UI_LIST_ON_CLOSE].func;
    void*                 arg         = _callback[UI_LIST_ON_CLOSE].user_data;

    callback_fn(arg);
    Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
  }
}

void ui_list_hook(u32 event_id, Callback callback)
{
  ASSERT(event_id < UI_LIST_NUM_EVENTS && "invalid event_id");
  _callback[event_id] = callback;
}
