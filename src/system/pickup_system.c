#include "pickup_system.h"
#include "mediator.h"
#include <components.h>

static void on_player_hit_item(Ecs* ecs, const SysEvt_PlayerHitItem* event)
{
  ecs_add(ecs, event->item, TAG_TO_BE_DESTROYED); 
}
void pickup_system_init(Ecs * ecs)
{
  mediator_connect(SIG_PLAYER_HIT_ITEM, ecs, SLOT(on_player_hit_item));
}
