#include "inventory.h"

#include "global.h"
#include "constances.h"
#include "engine/keyboard.h"
#include "entity_utils.h"
#include "game_scene.h"
#include "resources.h"
#include "ui_helper.h"
#include "ui_list.h"

#define INV_CELL_SIZE 16
#define INV_CELL_GAP 1
#define INV_X 16
#define INV_Y 32
#define INV_COL 5
#define INV_ROW 5
#define INV_MAX_SLOTS 25
extern SDL_Renderer* g_renderer;

static Item         _items[NUM_ITEM_CATEGORIES][INV_MAX_SLOTS];
static s32          _curr_col, _curr_row;
static BOOL         _visible;
static ItemCategory _category = ITEM_CATEGORY_CONSUMABLE;

static const RECT _description_rect = { 100, INV_Y, 100, 25 };

static const char        INV_TEXT_USE[]  = "USE";
static const char        INV_TEXT_DROP[] = "DROP";
static const char* const INV_OPTIONS[]   = { INV_TEXT_USE, INV_TEXT_DROP };

//<----------------------------------event callbacks--------------------------->//
static void process_key_input(void);
static void on_list_selected(pointer_t arg, const char* item);
//<============================================================================>//

static Item* current_item(void)
{
  return &_items[_category][_curr_row * INV_COL + _curr_col];
}

static int find_item(ItemTypeId type_id)
{
  const ItemCategory item_category = g_item_types[type_id].category;
  for (u32 i = 0; i < INV_MAX_SLOTS; ++i)
    if (_items[item_category][i].type_id == type_id)
      return i;
  return -1;
}

static int find_empty_slot(u16 category)
{
  for (u32 i = 0; i < INV_MAX_SLOTS; ++i)
    if (_items[category][i].num_items == 0)
      return i;
  return -1;
}

static void play_sound(SfxId id)
{
  Mix_PlayChannel(-1, get_sfx(id), 0);
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_LEFT))
  {
    _curr_col = max(0, _curr_col - 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_RIGHT))
  {
    _curr_col = min(INV_COL - 1, _curr_col + 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_DOWN))
  {
    _curr_row = min(INV_ROW - 1, _curr_row + 1);
    play_sound(SFX_BUTTON);
  }
  if (key_just_pressed(KEY_UP))
  {
    _curr_row = max(0, _curr_row - 1);
    play_sound(SFX_BUTTON);
  }

  if (key_just_pressed(KEY_A) && current_item()->num_items > 0)
  {
    play_sound(SFX_INTERACTION);
    ui_list_display((const char**)INV_OPTIONS, 2);
    ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_selected));
  }

  if (key_just_pressed(KEY_L) && _category > 0)
  {
    --_category;
    play_sound(SFX_INTERACTION);
  }

  if (key_just_pressed(KEY_R) && _category < NUM_ITEM_CATEGORIES - 1)
  {
    ++_category;
    play_sound(SFX_INTERACTION);
  }

  if (key_just_pressed(KEY_B))
  {
    inventory_close();
    play_sound(SFX_INTERACTION);
  }
}

static void on_list_selected(pointer_t arg, const char* item)
{
  (void)arg;
  Item*           it   = current_item();
  const ItemType* type = &g_item_types[it->type_id];

  if (strcmp(item, INV_TEXT_USE) == 0)
  {
    if (type->category == ITEM_CATEGORY_CONSUMABLE)
      it->num_items--;
    type->use(type->data, g_ecs, get_player(g_ecs));
  }
  else if (strcmp(item, INV_TEXT_DROP) == 0)
  {
    it->num_items = 0;
  }
}

BOOL add_to_inv(ItemTypeId type_id, u8 quality)
{
  ItemCategory cat   = g_item_types[type_id].category;
  Item*        items = _items[cat];
  int          idx   = find_item(type_id);
  int          empty_slot;

  if (idx == -1)
  {
    empty_slot = find_empty_slot(cat);
    if (empty_slot != -1)
    {
      items[empty_slot] = (Item){ .type_id = type_id, .num_items = quality };
      return TRUE;
    }
    return FALSE;
  }
  else
  {
    if (items[idx].num_items < ITEM_MAX_STACK)
    {
      items[idx].num_items = min(items[idx].num_items + quality, ITEM_MAX_STACK);
      return TRUE;
    }
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
  keybroad_push_state(process_key_input);
}

void inventory_close()
{
  game_scene_resume();
  _visible = FALSE;
  keybroad_pop_state();
}

void inventory_draw()
{
  if (!_visible)
    return;

  RECT cell_rect;
  int  idx;

  Item*           items = _items[_category];
  const ItemType* tp;

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
      draw_bordered_box(&cell_rect,
                        UI_COLOR_BG,
                        i == _curr_row && j == _curr_col ? UI_COLOR_BORDER_SELECT
                                                         : UI_COLOR_BORDER);

      idx = i * INV_COL + j;

      tp = &g_item_types[items[idx].type_id];

      if (items[idx].num_items > 0)
      {
        SDL_RenderCopy(g_renderer,
                       get_texture(tp->icon.texture_id),
                       &tp->icon.rect,
                       &cell_rect);
        if (items[idx].num_items > 1)
          FC_DrawColor(get_font(FONT_DAMAGE_INDICATOR),
                       g_renderer,
                       cell_rect.x + 3,
                       cell_rect.y + 3,
                       (COLOR){ 232, 39, 194, 255 },
                       "%d",
                       items[idx].num_items);
      }
    }
  }

  idx = _curr_col + _curr_row * INV_COL;

  draw_bordered_box(&_description_rect, UI_COLOR_BG, UI_COLOR_BORDER);
  if (items[idx].num_items > 0)
  {
    FC_DrawBoxColor(get_font(FONT_DAMAGE_INDICATOR),
                    g_renderer,
                    (RECT){
                        _description_rect.x + 3,
                        _description_rect.y + 2,
                        _description_rect.w - 4,
                        _description_rect.h - 5,
                    },
                    UI_COLOR_TEXT,
                    g_item_types[items[idx].type_id].description);
  }
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
