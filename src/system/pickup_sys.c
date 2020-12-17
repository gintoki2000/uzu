#include "pickup_sys.h"

#include "mediator.h"
#include <components.h>
#include <inventory.h>
#include <resources.h>

static void on_player_hit_item(Ecs* ecs, const SysEvt_PlayerHitItem* event)
{
  ItemTag* item_tag;
  if ((item_tag = ecs_get(ecs, event->item, ITEM_TAG)) != NULL)
  {
    if (add_to_inv(item_tag->item_id))
    {
      Mix_PlayChannel(-1, get_sfx(SFX_INTERACTION), 0);
      ecs_add(ecs, event->item, TAG_TO_BE_DESTROYED);
    }
  }
}
void pickup_system_init(Ecs* ecs)
{
  mediator_connect(SIG_PLAYER_HIT_ITEM, ecs, SLOT(on_player_hit_item));
}
