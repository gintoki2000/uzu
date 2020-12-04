#include "health_system.h"
#include "mediator.h"
#include <components.h>

static void on_deal_damage(Ecs* ecs, const SysEvt_DealDamage* event)
{
  Heath* health;
  if ((health = ecs_get(ecs, event->receiver, HEATH)) != NULL)
  {
    health->hit_points -= event->damage;
    if (health->hit_points <= 0)
    {
      health->hit_points = 0;
      mediator_emit(SIG_ENTITY_DIED, &(SysEvt_EntityDied){ event->receiver });
      ecs_add(ecs, event->receiver, TAG_TO_BE_DESTROYED);
    }
  }
}

void health_system_init(Ecs* ecs)
{
  mediator_connect(SIG_DEAL_DAMAGE, ecs, SLOT(on_deal_damage));
}
