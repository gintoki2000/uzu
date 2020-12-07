#include "inventory.h"
#include "item.h"
#include "resources.h"

#define MAX_NUM_ITEMS 10

static Item _item_db[NUM_ITEMS] = {
  [ITEM_RED_FLASK] =
      (Item){
          .name        = "red flask",
          .description = "restore a small amount of hp",
      },
  [ITEM_BIG_RED_FLASK] =
      (Item){
          .name        = "big red flask",
          .description = "restore a large amount of hp",
      },
};

static ItemId _items[MAX_NUM_ITEMS];
static u32    _item_cnt;

void inventory_init()
{
  sprite_init(&_item_db[ITEM_RED_FLASK].sprite, get_texture(TEX_FLASK_RED));
  sprite_init(&_item_db[ITEM_BIG_RED_FLASK].sprite, get_texture(TEX_FLASK_RED_BIG));
}

BOOL add_to_inv(ItemId item_id)
{
  if (_item_cnt < MAX_NUM_ITEMS)
  {
    _items[_item_cnt++] = item_id;

    INFO("%s added to inventory\n", _item_db[item_id].name);
    return TRUE;
  }
  return FALSE;
}

const Item* get_item_form_inv(ItemId item_id) { return &_item_db[item_id]; }

u32 get_mul_item_form_inv(const Item** items, u32 start_index, u32 cnt)
{
  u32 c = 0;
  for (u32 i = start_index; (i < _item_cnt) && (i < start_index + cnt); ++i, ++c)
  {
    items[i] = get_item_form_inv(_items[i]);
  }
  return c;
}
