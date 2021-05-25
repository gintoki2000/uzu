#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "game_scene.h"
#include "global.h"
#include "inventory.h"
#include "resources.h"
#include "session.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

static void item_handler(ecs_entity_t, const PickupableAttributes*, const Vec2 position);
static void coin_handler(ecs_entity_t, const PickupableAttributes*, const Vec2 position);

static void (*const handlerFuncTbl[])(ecs_entity_t, const PickupableAttributes*, const Vec2) = {
  [PICKUPABLE_RED_FLASK] = item_handler,  [PICKUPABLE_BIG_RED_FLASK] = item_handler,
  [PICKUPABLE_BLUE_FLASK] = item_handler, [PICKUPABLE_KEY_1_1] = item_handler,
  [PICKUPABLE_COIN] = coin_handler,
};

static void on_hit_pickupable_entity(SDL_UNUSED void* arg, const HitPickupableEntityMsg* msg)
{
  PickupableAttributes* attrs     = ecs_get(gEcs, msg->pickupableEntity, PICKUPABLE_ATTRIBUTES);
  Transform*            transform = ecs_get(gEcs, msg->pickupableEntity, TRANSFORM);

  void (*handler)(ecs_entity_t, const PickupableAttributes*, const Vec2);

  if ((handler = handlerFuncTbl[attrs->id], handler != NULL))
    handler(msg->pickupableEntity, attrs, transform->position);
}

static void
item_handler(ecs_entity_t entity, const PickupableAttributes* attrs, const Vec2 position)
{
  u16 itemTypeId = gPickupableToItemTypeIdTbl[attrs->id];
  if (itemTypeId != ITEM_TYPE_ID_NULL)
  {
    if (inv_add_item(itemTypeId, attrs->quality))
    {
      ems_broadcast(MSG_ITEM_PICKED_UP,
                    &(ItemPickedUpMsg){ .pickupable_entity = entity,
                                        .item_type_id      = itemTypeId,
                                        .position          = position });
      ecs_add(gEcs, entity, DESTROYED_TAG);
    }
  }
}

static void
coin_handler(ecs_entity_t entity, const PickupableAttributes* attrs, const Vec2 position)
{
  gSession.coins += attrs->quality;
  ems_broadcast(MSG_COIN_PICKED_UP, &(CoinPickedUpMsg){ attrs->quality, position });
  ecs_add(gEcs, entity, DESTROYED_TAG);
}
void pickup_system_init()
{
  ems_connect(MSG_PLAYER_HIT_ITEM, CALLBACK_2(on_hit_pickupable_entity));
}
