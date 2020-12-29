#ifndef ITEMS_H
#define ITEMS_H
#include <toolbox/toolbox.h>

typedef enum
{
  ITEM_TYPE_RED_FLASK,
  ITEM_TYPE_BIG_RED_FLASK,
  NUM_ITEM_TYPES,
} ItemTypeId;

typedef struct
{
  const char* name;
  const char* description;
  Sprite      sprite;
  BOOL        stackable;
} ItemType;

typedef struct
{
  ItemTypeId type_id;
  int        cnt;
} Item;

extern ItemType g_item_types[];

void item_types_init(void);

#endif // ITEMS_H
