#ifndef INVENTORY_H
#define INVENTORY_H
#include "global.h"
void inventory_init();
BOOL  add_item_to_inventory(ItemTypeId type_id, u8 quality);
BOOL  has_item_in_inventory(ItemTypeId type_id);
u8    get_item_count_in_inventory(ItemTypeId type_id);
Item* get_item(ItemTypeId type_id);
void inventory_display(void);
void inventory_close(void);
void inventory_draw(void);
void inventory_save(void);
void inventory_load(void);

#endif // INVENTORY_H
