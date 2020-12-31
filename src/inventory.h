#ifndef INVENTORY_H
#define INVENTORY_H
#include "item.h"
void  inventory_init();
BOOL  add_to_inv(ItemTypeId type_id);

void inventory_display(void);
void inventory_close(void);
void inventory_draw(void);

#endif // INVENTORY_H
