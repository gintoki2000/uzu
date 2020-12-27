#include "pickup_sys.h"

#include "mediator.h"
#include <components.h>
#include <game_scene.h>
#include <inventory.h>
#include <resources.h>

static void on_player_hit_item(void* arg, const SysEvt_PlayerHitItem* event)
{
  (void)arg;
  ItemTag* item_tag;
  if ((item_tag = ecs_get(g_ecs, event->item, ITEM_TAG)) != NULL)
  {
    if (add_to_inv(item_tag->item_id))
    {
      Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
      ecs_add(g_ecs, event->item, TAG_TO_BE_DESTROYED);
    }
  }
}

void pickup_system_init()
{
  mediator_connect(SYS_SIG_PLAYER_HIT_ITEM, NULL, SLOT(on_player_hit_item));
}
