#ifndef ITEMS_H
#define ITEMS_H
#include "ecs/ecs.h"
#include <toolbox/toolbox.h>

#define ITEM_MAX_STACK 255

typedef enum
{
  ITEM_TYPE_RED_FLASK,
  ITEM_TYPE_BIG_RED_FLASK,
  ITEM_TYPE_BLUE_FLASK,
  NUM_ITEM_TYPES,
} ItemTypeId;

typedef enum
{
  ITEM_CATEGORY_CONSUMABLE,
  ITEM_CATEGORY_EQUIPMENT,
  NUM_ITEM_CATEGORIES,
} ItemCategory;

typedef struct
{
  const char*  name;
  const char*  description;
  Sprite       sprite;
  BOOL         stackable;
  ItemCategory category;
  void (*on_use)(Ecs*, ecs_entity_t);

} ItemType;

typedef struct
{
  ItemTypeId type_id;
  u8         cnt;
} Item;

extern ItemType g_item_types[];

void item_types_init(void);

#endif // ITEMS_H
