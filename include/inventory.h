#ifndef INVENTORY_H
#define INVENTORY_H
#include "global.h"
BOOL  inv_add_item(ItemTypeId type, u8 quality);
BOOL  inv_has_item(ItemTypeId type);
u8    inv_item_count(ItemTypeId type);
Item* inv_get_item(ItemTypeId type);

void inventory_init();
void inventory_display(void);
void inventory_close(void);
void inventory_draw(void);
void inventory_save(void);
void inventory_load(void);

#endif // INVENTORY_H
