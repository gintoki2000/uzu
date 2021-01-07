#include "components.h"
#include "resources.h"
#include "types.h"

static void red_flask_on_use(Ecs* ecs, ecs_entity_t entity);
static void big_red_flask_on_use(Ecs* ecs, ecs_entity_t entity);
static void blue_flask_on_use(Ecs* ecs, ecs_entity_t entity);

ItemType g_item_types[NUM_ITEM_TYPES] = {
  [ITEM_TYPE_RED_FLASK] =
      {
          .name        = "red flask",
          .description = "restore a small amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .on_use      = red_flask_on_use,
      },
  [ITEM_TYPE_BIG_RED_FLASK] =
      {
          .name        = "big red flask",
          .description = "restore a large amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .on_use      = big_red_flask_on_use,
      },
  [ITEM_TYPE_BLUE_FLASK] =
      {
          .name        = "blue flask",
          .description = "restore a small amount of mp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .on_use      = blue_flask_on_use,
      },
};

static void red_flask_on_use(Ecs* ecs, ecs_entity_t entity)
{
  Health* health = ecs_get(ecs, entity, HEALTH);

  if (health != NULL)
  {
    health->hit_points = min(health->max_hit_points, health->hit_points + 5);
  }
}
static void big_red_flask_on_use(Ecs* ecs, ecs_entity_t entity)
{
  Health* health = ecs_get(ecs, entity, HEALTH);

  if (health != NULL)
  {
    health->hit_points = min(health->max_hit_points, health->hit_points + 10);
  }
}
static void blue_flask_on_use(Ecs* ecs, ecs_entity_t entity)
{
  (void)ecs;
  (void)entity;
  INFO("nothing happen...\n");
}

void item_types_init()
{
  sprite_init(&g_item_types[ITEM_TYPE_RED_FLASK].sprite, get_texture(TEX_FLASK_RED));
  sprite_init(&g_item_types[ITEM_TYPE_BIG_RED_FLASK].sprite, get_texture(TEX_FLASK_RED_BIG));
  sprite_init(&g_item_types[ITEM_TYPE_BLUE_FLASK].sprite, get_texture(TEX_BLUE_FLASK));
}
