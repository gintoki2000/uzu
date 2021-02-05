#include "system/game_logic.h"
#include "components.h"
#include "entity_factory.h"
#include "entity_utils.h"
#include "game_scene.h"
#include "global.h"
#include "inventory.h"
#include "resources.h"
#include "session.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

static void callback1(ecs_entity_t, const PickupableAttributes*, const Vec2 position);
static void callback2(ecs_entity_t, const PickupableAttributes*, const Vec2 position);

static void (*const handler_callback_tbl[])(ecs_entity_t,
                                            const PickupableAttributes*,
                                            const Vec2) = {
  [PICKUPABLE_RED_FLASK] = callback1,  [PICKUPABLE_BIG_RED_FLASK] = callback1,
  [PICKUPABLE_BLUE_FLASK] = callback1, [PICKUPABLE_KEY_1_1] = callback1,
  [PICKUPABLE_COIN] = callback2,
};

static void on_hit_pickupable_entity(SDL_UNUSED void* arg, const MSG_HitPickupableEntity* event)
{
  PickupableAttributes* attrs     = ecs_get(g_ecs, event->pickupable_entity, PICKUPABLE_ATTRIBUTES);
  Transform*            transform = ecs_get(g_ecs, event->pickupable_entity, TRANSFORM);

  void (*handler)(ecs_entity_t, const PickupableAttributes*, const Vec2);

  handler = handler_callback_tbl[attrs->id];

  if (handler)
    handler(event->pickupable_entity, attrs, transform->position);
}

static void callback1(ecs_entity_t entity, const PickupableAttributes* attrs, const Vec2 position)
{
  u16 item_type_id = g_pickupable_to_item_type_id_tbl[attrs->id];
  if (item_type_id != ITEM_TYPE_ID_NULL)
  {
    if (add_to_inv(item_type_id, 1))
    {
      ems_broadcast(MSG_ITEM_PICKED_UP,
                    &(MSG_ItemPickedUp){ .pickupable_entity = entity,
                                         .item_type_id      = item_type_id,
                                         .position          = position });
      ecs_add(g_ecs, entity, DESTROYED_TAG);
    }
  }
}

static void callback2(ecs_entity_t entity, const PickupableAttributes* attrs, const Vec2 position)
{
  g_session.coins += attrs->coins;
  ems_broadcast(MSG_COIN_PICKED_UP, &(MSG_CoinPickedUp){ attrs->coins, position });
  ecs_add(g_ecs, entity, DESTROYED_TAG);
}
void pickup_system_init()
{
  ems_connect(MSG_PLAYER_HIT_ITEM, NULL, on_hit_pickupable_entity);
}
