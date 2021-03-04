#include "inventory.h"

#include "constances.h"
#include "dungeon.h"
#include "engine/keyboard.h"
#include "entity_utils.h"
#include "game_scene.h"
#include "global.h"
#include "resources.h"
#include "ui_helper.h"
#include "ui_list.h"

#define CELL_SIZE 16
#define CELL_GAP 9
#define INV_X 60
#define INV_Y 25
#define NUM_COLS 7
#define NUM_ROWS 3
#define MAX_CELLS 21
#define FIRST_CELL_X (INV_X + 17)
#define FIRST_CELL_Y (INV_Y + 37)
#define ITEM_NAME_X (INV_X + 97)
#define ITEM_NAME_Y (INV_Y + 14)
#define CURSOR_WIDTH 26
#define CURSOR_HEIGHT 26
#define CURSOR_FRAME_DURATION 10
#define CURSOR_NUM_FRAMES 6

static RECT _cell_panel = { FIRST_CELL_X,
                            FIRST_CELL_Y,
                            (CELL_SIZE + CELL_GAP) * NUM_COLS,
                            (CELL_SIZE + CELL_GAP) * NUM_ROWS };

extern RENDERER* g_renderer;

static Item         _items[NUM_ITEM_CATEGORIES][MAX_CELLS];
static s32          _curr_col, _curr_row;
static BOOL         _visible;
static ItemCategory _category = ITEM_CATEGORY_CONSUMABLE;
static TEXTURE*     _inventory_texture;
static TEXTURE*     _icon[NUM_ITEM_CATEGORIES];
static TEXTURE*     _active_tab_texture;
static FONT*        _font;
static TEXTURE*     _cursor_texture;
static u32          _cursor_ticks;

static const RECT _description_rect = { INV_X + 27, INV_Y + 120, 147, 47 };

static const RECT _icon_dst_tbl[NUM_ITEM_CATEGORIES] = {
  { INV_X + 12, INV_Y + 5, 16, 16 },
  { INV_X + 37, INV_Y + 5, 16, 16 },
  { INV_X + 147, INV_Y + 5, 16, 16 },
  { INV_X + 172, INV_Y + 5, 16, 16 },
};

static const FC_Effect _item_name_effect = {
  .alignment = FC_ALIGN_CENTER,
  .color     = { 0x4a, 0x3d, 0x3c, 0xff },
  .scale     = { 1.f, 1.f },
};

static const RECT _active_tab_dst_tbl[] = {
  { INV_X + 10, INV_Y - 1, 20, 23 },
  { INV_X + 35, INV_Y - 1, 20, 23 },
  { INV_X + 145, INV_Y - 1, 20, 23 },
  { INV_X + 170, INV_Y - 1, 20, 23 },
};

static const RECT _cursor_frame_rects[] = {
  { 0 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 1 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 2 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 3 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 4 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 5 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
};

static const RECT _inventory_dst = { INV_X, INV_Y, 200, 190 };

static const char        _text_use[]  = "USE";
static const char        _text_drop[] = "DROP";
static const char* const _opts[]      = { _text_use, _text_drop };

//<----------------------------------event callbacks--------------------------->//
static void process_input(void*);
static void on_list_selected(pointer_t arg, const char* item);
//<============================================================================>//

static Item* current_item(void)
{
  return &_items[_category][_curr_row * NUM_COLS + _curr_col];
}

static int find_item(ItemTypeId type_id)
{
  const ItemCategory item_category = g_item_types[type_id].category;
  for (u32 i = 0; i < MAX_CELLS; ++i)
    if (_items[item_category][i].type_id == type_id)
      return i;
  return -1;
}

static int find_empty_slot(u16 category)
{
  for (u32 i = 0; i < MAX_CELLS; ++i)
    if (_items[category][i].num_items == 0)
      return i;
  return -1;
}

static void play_sound(SfxId id)
{
  Mix_PlayChannel(-1, get_sfx(id), 0);
}

static void process_input(SDL_UNUSED void* arg)
{
  SDL_Point mouse_position;
  SDL_GetMouseState(&mouse_position.x, &mouse_position.y);
  mouse_position.x /= SCL_X;
  mouse_position.y /= SCL_Y;

  if (SDL_PointInRect(&mouse_position, &_cell_panel))
  {
    SDL_Point cell_position = mouse_position;
    cell_position.x -= FIRST_CELL_X;
    cell_position.y -= FIRST_CELL_Y;

    cell_position.x /= (CELL_SIZE + CELL_GAP);
    cell_position.y /= (CELL_SIZE + CELL_GAP);
    _curr_col = cell_position.x;
    _curr_row = cell_position.y;
  }
  if (button_just_pressed(BUTTON_LEFT))
  {
    _curr_col = max(0, _curr_col - 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_RIGHT))
  {
    _curr_col = min(NUM_COLS - 1, _curr_col + 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_DOWN))
  {
    _curr_row = min(NUM_ROWS - 1, _curr_row + 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_UP))
  {
    _curr_row = max(0, _curr_row - 1);
    play_sound(SFX_BUTTON);
  }

  if (button_just_pressed(BUTTON_INTERACT) && current_item()->num_items > 0)
  {
    play_sound(SFX_INTERACTION);
    ui_list_display((const char**)_opts, 2);
    ui_list_set_pos(UI_LIST_POS_CENTER_X, UI_LIST_POS_CENTER_Y);
    ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_selected));
  }

  if (button_just_pressed(BUTTON_CANCEL))
  {
    inventory_close();
    play_sound(SFX_INTERACTION);
  }

  if (mouse_button_just_pressed(SDL_BUTTON_LEFT))
  {
    for (int i = 0; i < NUM_ITEM_CATEGORIES; ++i)
    {
      if (SDL_PointInRect(&mouse_position, &_icon_dst_tbl[i]))
      {
        _category = i;
        play_sound(SFX_INTERACTION);
      }
    }

    if (SDL_PointInRect(&mouse_position, &_cell_panel))
    {

      play_sound(SFX_INTERACTION);
      ui_list_display((const char**)_opts, 2);
      ui_list_set_pos(UI_LIST_POS_CENTER_X, UI_LIST_POS_CENTER_Y);
      ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_selected));
    }
  }
}

static void on_list_selected(SDL_UNUSED pointer_t arg, const char* value)
{
  Item*           it   = current_item();
  const ItemType* type = &g_item_types[it->type_id];

  if (STREQ(value, _text_use))
  {
    if (type->category == ITEM_CATEGORY_CONSUMABLE)
      it->num_items--;
    type->use(type->data, g_ecs, get_player(g_ecs));
  }
  else if (STREQ(value, _text_drop))
  {
    it->num_items = 0;
  }
}

void inventory_init()
{
  _inventory_texture              = get_texture(TEX_INVENTORY);
  _icon[ITEM_CATEGORY_CONSUMABLE] = get_texture(TEX_ICON_CONSUMABLE);
  _icon[ITEM_CATEGORY_EQUIPMENT]  = get_texture(TEX_ICON_WEAPON);
  _icon[ITEM_CATEGORY_SCROLL]     = get_texture(TEX_ICON_SPELL);
  _icon[ITEM_CATEGORY_KEY]        = get_texture(TEX_ICON_KEY_ITEM);
  _font                           = get_font(FONT_ITEM_PICKED_UP);
  _active_tab_texture             = get_texture(TEX_INVENTORY_TAB_ACTIVE);
  _cursor_texture                 = get_texture(TEX_INVENTORY_CURSOR);
}

#define is_stackable(__itemptr) (g_item_types[(__itemptr)->type_id].stackable)

#define item_name(__itemptr) (g_item_types[(__itemptr)->type_id].name)

#define item_desc(__itemptr) (g_item_types[(__itemptr)->type_id].description)

#define item_icon(__itemptr) (g_item_types[(__itemptr)->type_id].icon)

BOOL add_to_inv(ItemTypeId type_id, u8 quality)
{
  ItemCategory cat   = g_item_types[type_id].category;
  Item*        items = _items[cat];
  int          idx;
  int          empty_slot;

  if (type_id == ITEM_TYPE_ID_NULL)
    return FALSE;

  idx = find_item(type_id);

  if (idx != -1 && is_stackable(&items[idx]) && items[idx].num_items < ITEM_MAX_STACK)
  {
    items[idx].num_items = min(items[idx].num_items + quality, ITEM_MAX_STACK);
    return TRUE;
  }
  else
  {
    empty_slot = find_empty_slot(cat);
    if (empty_slot != -1)
    {
      items[empty_slot] = (Item){ .type_id = type_id, .num_items = quality };
      return TRUE;
    }
    // inventory is full
    return FALSE;
  }
  return FALSE;
}

BOOL has_item(ItemTypeId type_id)
{
  return find_item(type_id) != -1;
}

Item* get_item(ItemTypeId type_id)
{
  u16 category = g_item_types[type_id].category;
  int index    = find_item(type_id);
  if (index != -1)
  {
    return &_items[category][index];
  }
  else
  {
    return NULL;
  }
}

u8 get_item_count(ItemTypeId type_id)
{
  const Item* item = get_item(type_id);
  return item ? item->num_items : 0;
}

void inventory_display()
{
  game_scene_pause();
  _visible = TRUE;
  input_push_state(INPUT_STATE_INST_1(process_input));
  extern Cursor g_cursor_pointer;
  push_cursor_state(g_cursor_pointer);
}

void inventory_close()
{
  game_scene_resume();
  _visible = FALSE;
  input_pop_state();
  pop_cursor_state();
}

static void draw_cursor(void)
{
  int  current_frame;
  RECT dst;

  ++_cursor_ticks;

  current_frame = (_cursor_ticks / CURSOR_FRAME_DURATION) % CURSOR_NUM_FRAMES;

  dst.x = FIRST_CELL_X + _curr_col * (CELL_GAP + CELL_SIZE) + (CELL_SIZE / 2 - CURSOR_WIDTH / 2);
  dst.y = FIRST_CELL_Y + _curr_row * (CELL_GAP + CELL_SIZE) + (CELL_SIZE / 2 - CURSOR_HEIGHT / 2);
  dst.w = CURSOR_WIDTH;
  dst.h = CURSOR_HEIGHT;

  SDL_RenderCopy(g_renderer, _cursor_texture, &_cursor_frame_rects[current_frame], &dst);
}

void inventory_draw()
{
  RECT            item_rect;
  int             idx;
  Item*           items = _items[_category];
  const ItemType* item_type;
  Icon            item_icon;

  if (!_visible)
    return;

  SDL_RenderCopy(g_renderer, _inventory_texture, NULL, &_inventory_dst);

  item_rect.w = 16;
  item_rect.h = 16;
  for (int i = 0; i < NUM_ROWS; ++i)
  {
    for (int j = 0; j < NUM_COLS; ++j)
    {
      idx = i * NUM_COLS + j;

      if (items[idx].num_items == 0)
        continue;

      item_rect.x = FIRST_CELL_X + j * (16 + CELL_GAP);
      item_rect.y = FIRST_CELL_Y + i * (16 + CELL_GAP);

      item_type = &g_item_types[items[idx].type_id];
      item_icon = item_type->icon;

      SDL_RenderCopy(g_renderer, get_texture(item_icon.texture_id), &item_icon.rect, &item_rect);
    }
  }
  int current_item_index = _curr_row * NUM_COLS + _curr_col;
  if (items[current_item_index].num_items > 0)
  {
    item_type = &g_item_types[items[current_item_index].type_id];
    FC_DrawEffect(_font, g_renderer, ITEM_NAME_X, ITEM_NAME_Y, _item_name_effect, item_type->name);
    FC_DrawBoxEffect(_font,
                     g_renderer,
                     _description_rect,
                     _item_name_effect,
                     item_type->description);
  }

  SDL_RenderCopy(g_renderer, _active_tab_texture, NULL, &_active_tab_dst_tbl[_category]);
  for (int i = 0; i < NUM_ITEM_CATEGORIES; ++i)
  {
    SDL_RenderCopy(g_renderer, _icon[i], NULL, &_icon_dst_tbl[i]);
  }

  draw_cursor();
}

void inventory_save(void)
{
  FILE* file = fopen("inventory", "w");
  if (file != NULL)
  {
    fwrite(_items, sizeof(_items), 0, file);
    fclose(file);
  }
}

void inventory_load(void)
{
  FILE* file = fopen("inventory", "r");
  if (file != NULL)
  {
    fread(_items, sizeof(_items), 0, file);
    fclose(file);
  }
}
