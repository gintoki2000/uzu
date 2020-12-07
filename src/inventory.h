#ifndef INVENTORY_H
#define INVENTORY_H
#include "item.h"
void        inventory_init();
void        inventory_fini();
BOOL        add_to_inv(ItemId item_id);
const Item* get_item_form_inv(u32 index);
u32         get_mul_item_form_inv(const Item** items, u32 start_index, u32 cnt);
void        remove_item_from_inv();

#endif // INVENTORY_H
