#ifndef ITEMS_H
#define ITEMS_H
#include <toolbox/toolbox.h>

typedef enum
{
  ITEM_RED_FLASK,
  ITEM_BIG_RED_FLASK,
  NUM_ITEMS,
} ItemId;

typedef struct
{
  const char* name;
  const char* description;
  Sprite      sprite;
} Item;


#endif // ITEMS_H
