#include "components.h"
#include "resources.h"
#include "types.h"
#include "ui_msgbox.h"

static void scroll_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity);
static void healing_item_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity);
static void do_nothing_callback(const void*, Ecs*, ecs_entity_t);

typedef struct ScrollData
{
  u16 spell_id;
} ScrollData;

typedef struct HealingItemData
{
  u16 hp;
  u16 mp;
} HealingItemData;

static const ScrollData _ice_arrow_scroll_data = { SPELL_ICE_ARROW };
static const ScrollData _fire_ball_scroll_data = { SPELL_FIRE_BALL };

static const HealingItemData _red_flask_data     = { .hp = 5 };
static const HealingItemData _blue_flask_data    = { .mp = 5 };
static const HealingItemData _big_red_flask_data = { .hp = 10 };

const ItemType g_item_types[NUM_ITEM_TYPES] = {
  [ITEM_TYPE_RED_FLASK] =
      {
          .name        = "red flask",
          .description = "restore a small amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &_red_flask_data,
          .sprite      = {.texture_id = TEX_FLASK_RED, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_BIG_RED_FLASK] =
      {
          .name        = "big red flask",
          .description = "restore a large amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &_big_red_flask_data,
          .sprite      = {.texture_id = TEX_FLASK_RED_BIG, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_BLUE_FLASK] =
      {
          .name        = "blue flask",
          .description = "restore a small amount of mp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &_blue_flask_data,
          .sprite      = {.texture_id =  TEX_BLUE_FLASK, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_SCROLL_ICE_ARROW] = 
      {
          .name = "ice arrow scroll",
          .description = "cast ice arrow on straight line",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_SCROLL,
          .use = scroll_use_callback,
          .data = &_ice_arrow_scroll_data,
          .sprite = {.texture_id = TEX_SCROLL, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_SCROLL_FIRE_BALL] = 
      {
          .name = "fire ball scroll",
          .description = "cast fire ball on straight line",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_SCROLL,
          .use = scroll_use_callback,
          .data = &_fire_ball_scroll_data,
          .sprite = {.texture_id = TEX_SCROLL, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_KEY_1_1] =
      {
          .name = "level 1 door key 1",
          .description = "use to open door in level 0",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_KEY,
          .use = do_nothing_callback,
          .data = NULL,
          .sprite = {.texture_id = TEX_KEY, .rect = {0, 0, 16, 16}},
      },
};

static void healing_item_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity)
{
  const HealingItemData* healing_item_data = (const HealingItemData*)data;
  ManaPool*              mana_pool         = ecs_get(ecs, entity, MANA_POOL);
  Health*                health            = ecs_get(ecs, entity, HEALTH);
  if (health != NULL)
  {
    health->hit_points = min(health->max_hit_points, health->hit_points + healing_item_data->hp);
  }
  if (mana_pool != NULL)
  {
    mana_pool->mana_points =
        min(mana_pool->max_mana_points, mana_pool->mana_points + healing_item_data->mp);
  }
}

static void scroll_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity)
{
  const ScrollData* scroll_data     = (const ScrollData*)data;
  AttunementSlot*   attunement_slot = ecs_get(ecs, entity, ATTUNEMENT_SLOT);
  if (attunement_slot != NULL)
  {
    attunement_slot->spell_id = scroll_data->spell_id;
  }
}

static void do_nothing_callback(const void* data, Ecs* ecs, ecs_entity_t entity)
{
  (void)data;
  (void)ecs;
  (void)entity;
  ui_msgbox_display("not thing happen");
}
