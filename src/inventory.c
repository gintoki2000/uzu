#include "inventory.h"
#include "engine/keyboard.h"
#include "item.h"
#include "resources.h"
#include "ui_helper.h"

#define INV_CELL_SIZE 32
#define INV_CELL_GAP 1
#define INV_X 64
#define INV_Y 64
#define INV_COL 5
#define INV_ROW 5
#define INV_MAX_SLOT 25
extern SDL_Renderer* g_renderer;

static Item _items[NUM_ITEM_CATEGORIES][INV_MAX_SLOT];
static u32  _cnt[NUM_ITEM_CATEGORIES];
static s32  _selected;

static BOOL _visible;

static const COLOR _background_color = { 210, 189, 167, 255 };
static const COLOR _border_color     = { 169, 139, 121, 255 };
static const COLOR _text_color       = { 74, 61, 60, 255 };

static void process_key_input(void);

static int find_item(Item* items, u32 cnt, ItemTypeId type_id)
{
  for (u32 i = 0; i < cnt; ++i)
    if (items[i].type_id == type_id)
      return i;
  return -1;
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_B))
    inventory_close();
}

BOOL add_to_inv(ItemTypeId type_id)
{
  ItemCategory cat   = g_item_types[type_id].category;
  Item*        items = _items[cat];
  u32*         cnt   = &_cnt[cat];
  int          idx   = find_item(items, *cnt, type_id);

  if (idx == -1)
  {
    if (*cnt < INV_MAX_SLOT)
    {
      items[(*cnt)++] = (Item){ .type_id = type_id, .cnt = 1 };
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    if (items[idx].cnt < ITEM_MAX_STACK)
    {
      items[idx].cnt++;
      return TRUE;
    }
    return FALSE;
  }
  return FALSE;
}

void inventory_display()
{
  _visible = TRUE;
  keybroad_push_state(process_key_input);
}

void inventory_close()
{
  _visible = FALSE;
  keybroad_pop_state();
}

void inventory_draw()
{
  if (!_visible)
    return;

  RECT cell_rect;
  int  idx;

  Item*  items = _items[ITEM_CATEGORY_CONSUMABLE];
  Sprite sprite;

  for (int i = 0; i < INV_ROW; ++i)
  {
    for (int j = 0; j < INV_COL; ++j)
    {
      cell_rect = (RECT){
        j * INV_CELL_SIZE + INV_CELL_GAP + INV_X,
        i * INV_CELL_SIZE + INV_CELL_GAP + INV_Y,
        INV_CELL_SIZE,
        INV_CELL_SIZE,
      };
      draw_box_w_border(&cell_rect, _background_color, _border_color);

      idx    = i * INV_COL + j;
      sprite = g_item_types[items[idx].type_id].sprite;

      if (items[idx].cnt > 0)
      {
        SDL_RenderCopy(g_renderer, sprite.tex, &sprite.rect, &cell_rect);
        FC_DrawColor(get_font(FONT_DAMAGE_INDICATOR),
                     g_renderer,
                     cell_rect.x + 3,
                     cell_rect.y + 3,
                     (COLOR){ 232, 39, 194, 255 },
                     "%d",
                     items[idx].cnt);
      }
    }
  }
}
