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
static s32          _currCol, _currRow;
static BOOL         _visible;
static ItemCategory _category = ITEM_CATEGORY_CONSUMABLE;
static TEXTURE*     _inventoryTexture;
static TEXTURE*     _icon[NUM_ITEM_CATEGORIES];
static TEXTURE*     _activeTabTexture;
static FONT*        _font;
static TEXTURE*     _cursorTexture;
static u32          _cursorAnimationTicks;

static const RECT _descriptionRect = { INV_X + 27, INV_Y + 120, 147, 47 };

static const RECT _iconDstTbl[NUM_ITEM_CATEGORIES] = {
  { INV_X + 12, INV_Y + 5, 16, 16 },
  { INV_X + 37, INV_Y + 5, 16, 16 },
  { INV_X + 147, INV_Y + 5, 16, 16 },
  { INV_X + 172, INV_Y + 5, 16, 16 },
};

static const FC_Effect _itemNameEffect = {
  .alignment = FC_ALIGN_CENTER,
  .color     = { 0x4a, 0x3d, 0x3c, 0xff },
  .scale     = { 1.f, 1.f },
};

static const RECT _activeTabDstTbl[] = {
  { INV_X + 10, INV_Y - 1, 20, 23 },
  { INV_X + 35, INV_Y - 1, 20, 23 },
  { INV_X + 145, INV_Y - 1, 20, 23 },
  { INV_X + 170, INV_Y - 1, 20, 23 },
};

static const RECT _cursorFrameRects[] = {
  { 0 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 1 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 2 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 3 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 4 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
  { 5 * CURSOR_WIDTH, 0, CURSOR_WIDTH, CURSOR_HEIGHT },
};

static const RECT _inventoryDst = { INV_X, INV_Y, 200, 190 };

static const char        _textUse[]  = "USE";
static const char        _textDrop[] = "DROP";
static const char* const _options[]  = { _textUse, _textDrop };

//<----------------------------------event callbacks--------------------------->//
static void process_input(void*);
static void on_list_selected(pointer_t arg, const char* item);
//<============================================================================>//

static Item* current_item(void)
{
  return &_items[_category][_currRow * NUM_COLS + _currCol];
}

static int find_item(ItemTypeId type)
{
  const ItemCategory itemCategory = gItemTypes[type].category;
  for (u32 i = 0; i < MAX_CELLS; ++i)
    if (_items[itemCategory][i].type == type)
      return i;
  return -1;
}

static int find_empty_slot(u16 category)
{
  ASSERT(category < NUM_ITEM_CATEGORIES && "invalid category");
  for (u32 i = 0; i < MAX_CELLS; ++i)
    if (_items[category][i].quality == 0)
      return i;
  return -1;
}

static void play_sound(SfxId id)
{
  Mix_PlayChannel(-1, get_sfx(id), 0);
}

static void process_input(SDL_UNUSED void* arg)
{
  SDL_Point mousePosition;
  SDL_GetMouseState(&mousePosition.x, &mousePosition.y);
  mousePosition.x /= SCL_X;
  mousePosition.y /= SCL_Y;

  if (SDL_PointInRect(&mousePosition, &_cell_panel))
  {
    SDL_Point cell_position = mousePosition;
    cell_position.x -= FIRST_CELL_X;
    cell_position.y -= FIRST_CELL_Y;

    cell_position.x /= (CELL_SIZE + CELL_GAP);
    cell_position.y /= (CELL_SIZE + CELL_GAP);
    _currCol = cell_position.x;
    _currRow = cell_position.y;
  }
  if (button_just_pressed(BUTTON_LEFT))
  {
    _currCol = max(0, _currCol - 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_RIGHT))
  {
    _currCol = min(NUM_COLS - 1, _currCol + 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_DOWN))
  {
    _currRow = min(NUM_ROWS - 1, _currRow + 1);
    play_sound(SFX_BUTTON);
  }
  if (button_just_pressed(BUTTON_UP))
  {
    _currRow = max(0, _currRow - 1);
    play_sound(SFX_BUTTON);
  }

  if (button_just_pressed(BUTTON_INTERACT) && current_item()->quality > 0)
  {
    play_sound(SFX_INTERACTION);
    ui_list_display((const char**)_options, 2);
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
      if (SDL_PointInRect(&mousePosition, &_iconDstTbl[i]))
      {
        _category = i;
        play_sound(SFX_INTERACTION);
      }
    }

    if (SDL_PointInRect(&mousePosition, &_cell_panel) && current_item()->quality > 0)
    {
      play_sound(SFX_INTERACTION);
      ui_list_display((const char**)_options, 2);
      ui_list_set_pos(UI_LIST_POS_CENTER_X, UI_LIST_POS_CENTER_Y);
      ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_selected));
    }
  }
}

static void on_list_selected(SDL_UNUSED pointer_t arg, const char* value)
{
  Item*           it   = current_item();
  const ItemType* type = &gItemTypes[it->type];

  if (STREQ(value, _textUse))
  {
    if (type->category == ITEM_CATEGORY_CONSUMABLE)
      it->quality--;
    type->use(type->data, g_ecs, scn_get_player(g_ecs));
  }
  else if (STREQ(value, _textDrop))
  {
    it->quality = 0;
  }
}

void inventory_init()
{
  _inventoryTexture               = get_texture(TEX_INVENTORY);
  _icon[ITEM_CATEGORY_CONSUMABLE] = get_texture(TEX_ICON_CONSUMABLE);
  _icon[ITEM_CATEGORY_EQUIPMENT]  = get_texture(TEX_ICON_WEAPON);
  _icon[ITEM_CATEGORY_SCROLL]     = get_texture(TEX_ICON_SPELL);
  _icon[ITEM_CATEGORY_KEY]        = get_texture(TEX_ICON_KEY_ITEM);
  _font                           = get_font(FONT_ITEM_PICKED_UP);
  _activeTabTexture               = get_texture(TEX_INVENTORY_TAB_ACTIVE);
  _cursorTexture                  = get_texture(TEX_INVENTORY_CURSOR);
}

#define item_stackable(it) (gItemTypes[(it)->type].stackable)

#define item_name(it) (gItemTypes[(it)->type].name)

#define item_desc(it) (gItemTypes[(it)->type].description)

#define itemIcon(it) (gItemTypes[(it)->type].icon)

BOOL inv_add_item(ItemTypeId type, u8 quality)
{
  ItemCategory cat   = gItemTypes[type].category;
  Item*        items = _items[cat];
  int          idx;
  int          empty_slot;

  if (type == ITEM_TYPE_ID_NULL)
    return FALSE;

  idx = find_item(type);

  if (idx != -1 && item_stackable(&items[idx]) && items[idx].quality < ITEM_MAX_STACK)
  {
    items[idx].quality = min(items[idx].quality + quality, ITEM_MAX_STACK);
    return TRUE;
  }
  else
  {
    empty_slot = find_empty_slot(cat);
    if (empty_slot != -1)
    {
      items[empty_slot] = (Item){ .type = type, .quality = quality };
      return TRUE;
    }
    // inventory is full
    return FALSE;
  }
  return FALSE;
}

BOOL inv_has(ItemTypeId type)
{
  return find_item(type) != -1;
}

Item* inv_get_item(ItemTypeId type)
{
  u16 category = gItemTypes[type].category;
  int index    = find_item(type);
  return index != -1 ? &_items[category][index] : NULL;
}

u8 inv_count(ItemTypeId type)
{
  const Item* item = inv_get_item(type);
  return item ? item->quality : 0;
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

  ++_cursorAnimationTicks;

  current_frame = (_cursorAnimationTicks / CURSOR_FRAME_DURATION) % CURSOR_NUM_FRAMES;

  dst.x = FIRST_CELL_X + _currCol * (CELL_GAP + CELL_SIZE) + (CELL_SIZE / 2 - CURSOR_WIDTH / 2);
  dst.y = FIRST_CELL_Y + _currRow * (CELL_GAP + CELL_SIZE) + (CELL_SIZE / 2 - CURSOR_HEIGHT / 2);
  dst.w = CURSOR_WIDTH;
  dst.h = CURSOR_HEIGHT;

  SDL_RenderCopy(g_renderer, _cursorTexture, &_cursorFrameRects[current_frame], &dst);
}

void inventory_draw()
{
  RECT            itemCellRect;
  int             idx;
  Item*           items = _items[_category];
  const ItemType* itemType;
  Icon            itemIcon;

  if (!_visible)
    return;

  SDL_RenderCopy(g_renderer, _inventoryTexture, NULL, &_inventoryDst);

  itemCellRect.w = 16;
  itemCellRect.h = 16;
  for (int i = 0; i < NUM_ROWS; ++i)
  {
    for (int j = 0; j < NUM_COLS; ++j)
    {
      idx = i * NUM_COLS + j;

      if (items[idx].quality == 0)
        continue;

      itemCellRect.x = FIRST_CELL_X + j * (16 + CELL_GAP);
      itemCellRect.y = FIRST_CELL_Y + i * (16 + CELL_GAP);

      itemType = &gItemTypes[items[idx].type];
      itemIcon = itemType->icon;

      SDL_RenderCopy(g_renderer, get_texture(itemIcon.texture_id), &itemIcon.rect, &itemCellRect);
      if (items[idx].quality >= 1)
        FC_Draw(_font, g_renderer, itemCellRect.x, itemCellRect.y, "%d", items[idx].quality);
    }
  }
  int currentItemIndex = _currRow * NUM_COLS + _currCol;
  if (items[currentItemIndex].quality > 0)
  {
    itemType = &gItemTypes[items[currentItemIndex].type];
    FC_DrawEffect(_font, g_renderer, ITEM_NAME_X, ITEM_NAME_Y, _itemNameEffect, itemType->name);
    FC_DrawBoxEffect(_font, g_renderer, _descriptionRect, _itemNameEffect, itemType->description);
  }

  SDL_RenderCopy(g_renderer, _activeTabTexture, NULL, &_activeTabDstTbl[_category]);
  for (int i = 0; i < NUM_ITEM_CATEGORIES; ++i)
  {
    SDL_RenderCopy(g_renderer, _icon[i], NULL, &_iconDstTbl[i]);
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
