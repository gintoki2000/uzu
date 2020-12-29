#include "item.h"
#include "resources.h"

ItemType g_item_types[NUM_ITEM_TYPES] = {
  [ITEM_TYPE_RED_FLASK] =
      (ItemType){
          .name        = "red flask",
          .description = "restore a small amount of hp",
          .stackable   = TRUE,
      },
  [ITEM_TYPE_BIG_RED_FLASK] =
      (ItemType){
          .name        = "big red flask",
          .description = "restore a large amount of hp",
          .stackable   = TRUE,
      },
};

void item_types_init()
{
  sprite_init(&g_item_types[ITEM_TYPE_RED_FLASK].sprite, get_texture(TEX_FLASK_RED));
  sprite_init(&g_item_types[ITEM_TYPE_BIG_RED_FLASK].sprite, get_texture(TEX_FLASK_RED_BIG));
}
