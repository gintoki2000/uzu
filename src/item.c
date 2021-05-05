#include "components.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"
#include "ui_msgbox.h"

static void scroll_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity);
static void healing_item_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity);
static void do_nothing_callback(const void*, Ecs*, ecs_entity_t);
static void equipment_item_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity);

typedef struct ScrollData
{
  u16 spell_id;
} ScrollData;

typedef struct HealingItemData
{
  u16 hp;
  u16 mp;
} HealingItemData;

typedef struct EquipmentItemData
{
  u16 weapon_id;
} EquipmentItemData;

static const ScrollData k_scroll_data_ice_arrow = { SPELL_ICE_ARROW };
static const ScrollData k_scrool_data_fire_ball = { SPELL_FIRE_BALL };

static const HealingItemData k_healing_item_data_red_flask     = { .hp = 5 };
static const HealingItemData k_healing_item_data_blue_flask    = { .mp = 5 };
static const HealingItemData k_healing_item_data_big_red_flask = { .hp = 10 };

static const EquipmentItemData k_equipemt_item_data_spear       = { WEAPON_SPEAR };
static const EquipmentItemData k_equipemt_item_data_anime_sword = { WEAPON_ANIME_SWORD };
static const EquipmentItemData k_equipemt_item_data_staff       = { WEAPON_STAFF };
static const EquipmentItemData k_equipemt_item_data_cleaver     = { WEAPON_CLEAVER };

const ItemType g_item_types[NUM_ITEM_TYPES] = {
  [ITEM_TYPE_RED_FLASK] =
      {
          .name        = "red flask",
          .description = "restore a small amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &k_healing_item_data_red_flask,
          .icon      = {.texture_id = TEX_ITEM_FLASK_RED, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_BIG_RED_FLASK] =
      {
          .name        = "big red flask",
          .description = "restore a large amount of hp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &k_healing_item_data_big_red_flask,
          .icon      = {.texture_id = TEX_ITEM_FLASK_RED_BIG, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_BLUE_FLASK] =
      {
          .name        = "blue flask",
          .description = "restore a small amount of mp",
          .stackable   = TRUE,
          .category    = ITEM_CATEGORY_CONSUMABLE,
          .use      = healing_item_use_callback,
          .data = &k_healing_item_data_blue_flask,
          .icon      = {.texture_id =  TEX_ITEM_BLUE_FLASK, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_SCROLL_ICE_ARROW] = 
      {
          .name = "ice arrow scroll",
          .description = "cast ice arrow on straight line",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_SCROLL,
          .use = scroll_use_callback,
          .data = &k_scroll_data_ice_arrow,
          .icon = {.texture_id = TEX_ICON_ICE_ARROW, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_SCROLL_FIRE_BALL] = 
      {
          .name = "fire ball scroll",
          .description = "cast fire ball on straight line",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_SCROLL,
          .use = scroll_use_callback,
          .data = &k_scrool_data_fire_ball,
          .icon = {.texture_id = TEX_ICON_FIRE_BALL, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_KEY_1_1] =
      {
          .name = "level 1 door key 1",
          .description = "use to open door in level 0",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_KEY,
          .use = do_nothing_callback,
          .data = NULL,
          .icon = {.texture_id = TEX_ITEM_KEY, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_ANIME_SWORD] =
      {
          .name = "anime sword",
          .description = "anime sword",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_EQUIPMENT,
          .use = equipment_item_use_callback,
          .data = &k_equipemt_item_data_anime_sword,
          .icon = {.texture_id = TEX_ICON_CLEAVER, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_SPEAR] =
      {
          .name = "spear",
          .description = "spear",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_EQUIPMENT,
          .use = equipment_item_use_callback,
          .data = &k_equipemt_item_data_spear,
          .icon = {.texture_id = TEX_ICON_SPEAR, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_STAFF] =
      {
          .name = "staff",
          .description = "red staff",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_EQUIPMENT,
          .use = equipment_item_use_callback,
          .data = &k_equipemt_item_data_staff,
          .icon = {.texture_id = TEX_ICON_STAFF, .rect = {0, 0, 16, 16}},
      },
  [ITEM_TYPE_CLEAVER] =
      {
          .name = "cleaver",
          .description = "cleaver",
          .stackable = FALSE,
          .category  = ITEM_CATEGORY_EQUIPMENT,
          .use = equipment_item_use_callback,
          .data = &k_equipemt_item_data_cleaver,
          .icon = {.texture_id = TEX_ICON_CLEAVER, .rect = {-1, 0, 16, 16}},
      },
  
};

static void healing_item_use_callback(const void* data, Ecs* ecs, ecs_entity_t entity)
{
  const HealingItemData* healing_item_data = (const HealingItemData*)data;
  Mana*                  mana              = ecs_get(ecs, entity, MANA);
  Health*                health            = ecs_get(ecs, entity, HEALTH);
  if (health != NULL)
    health->current = min(health->max, health->current + healing_item_data->hp);
  if (mana != NULL)
    mana->current = min(mana->max, mana->current + healing_item_data->mp);
}

static void scroll_use_callback(const void* _data, Ecs* ecs, ecs_entity_t entity)
{
  const ScrollData* data            = (const ScrollData*)_data;
  AttunementSlot*   attunement_slot = ecs_get(ecs, entity, ATTUNEMENT_SLOT);
  if (attunement_slot != NULL)
  {
    attunement_slot->spell_id = data->spell_id;
  }
}

static void do_nothing_callback(SDL_UNUSED const void* data,
                                SDL_UNUSED Ecs*         ecs,
                                SDL_UNUSED ecs_entity_t entity)
{
  ui_msgbox_display("not thing happen");
}

static void equipment_item_use_callback(const void* _data, Ecs* ecs, ecs_entity_t entity)
{
  const EquipmentItemData* data = (const EquipmentItemData*)_data;
  ecs_entity_t             weapon;

  weapon = create_weapon(ecs, data->weapon_id);
  ett_equip_weapon(ecs, entity, weapon);
}
