#ifndef INVENTORY_H
#define INVENTORY_H
#include "item.h"
void  inventory_init();
void  inventory_fini();
BOOL  add_to_inv(ItemTypeId type_id);
Item* get_item_form_inv(u32 index);
u32   get_mul_item_form_inv(ItemType** items_ptr, u32 start_index, u32 cnt);
void  remove_item_from_inv();

#endif // INVENTORY_H
