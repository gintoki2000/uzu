#include "pickup_sys.h"
#include "components.h"
#include "entity_factory.h"
#include "event_messaging_sys.h"
#include "game_scene.h"
#include "inventory.h"
#include "resources.h"

extern Ecs* g_ecs;

static void on_player_hit_item(void* arg, const MSG_PlayerHitItem* event)
{
  (void)arg;
  ItemTag*   item_tag;
  Transform* transform;
  if ((item_tag = ecs_get(g_ecs, event->item, ITEM_TAG)) != NULL &&
      (transform = ecs_get(g_ecs, event->item, TRANSFORM)) != NULL)
  {
    INFO("%s\n", g_item_types[item_tag->item_id].name);
    if (add_to_inv(item_tag->item_id, 1))
    {
      Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
      make_item_picked_up_msg(g_ecs, transform->position, g_item_types[item_tag->item_id].name);

      ecs_add(g_ecs, event->item, TAG_TO_BE_DESTROYED);
    }
  }
}

void pickup_system_init()
{
  ems_connect(MSG_PLAYER_HIT_ITEM, NULL, on_player_hit_item);
}
