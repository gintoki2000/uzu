#include "inventory.h"
#include "item.h"
#include "resources.h"

#define MAX_NUM_ITEMS 10

static Item _items[MAX_NUM_ITEMS];
static u32  _item_cnt;

static s32 find(ItemTypeId type)
{
  for (u32 i = 0; i < _item_cnt; ++i)
    if (_items[i].type_id == type)
      return i;
  return -1;
}

void inventory_init()
{
}

BOOL add_to_inv(ItemTypeId type_id)
{
  int idx = find(type_id);
  if (idx == -1 && _item_cnt < MAX_NUM_ITEMS)
  {
    _items[_item_cnt++] = (Item){ .type_id = type_id, .cnt = 1 };
    return TRUE;
  }
  else
  {
    if (g_item_types[_items[idx].type_id].stackable)
    {
      _items[idx].cnt++;
      return TRUE;
    }
    else if (_item_cnt < MAX_NUM_ITEMS)
    {
      _items[_item_cnt++] = (Item){ .type_id = type_id, .cnt = 1 };
      return TRUE;
    }
  }
  return FALSE;
}

Item* get_item_form_inv(u32 type_id)
{
  return &_items[type_id];
}
