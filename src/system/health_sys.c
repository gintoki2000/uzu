#include "health_sys.h"

#include "mediator.h"
#include <components.h>

static void on_deal_damage(Ecs* ecs, const SysEvt_DealDamage* event)
{
  Heath* health;
  if ((health = ecs_get(ecs, event->receiver, HEATH)) != NULL &&
      !ecs_has(ecs, event->receiver, INVULNERABLE))
  {
    health->hit_points -= event->damage;
    INFO("take %d damage\n", event->damage);
    if (health->hit_points <= 0)
    {
      health->hit_points = 0;
      mediator_emit(SIG_ENTITY_DIED, &(SysEvt_EntityDied){ event->receiver });
      ecs_add(ecs, event->receiver, TAG_TO_BE_DESTROYED);
    }
    else
    {
      Invulnerable* invulnerable;
      mediator_emit(SIG_GET_DAMAGED,
                    &(SysEvt_GetDamaged){
                        .dealer  = event->dealer,
                        .damagee = event->receiver,
                        .damage  = event->damage,
                        .type    = event->type,
                    });
      invulnerable            = ecs_add(ecs, event->receiver, INVULNERABLE);
      invulnerable->remaining = 5;
    }
  }
}

void health_system_init(Ecs* ecs) { mediator_connect(SIG_DEAL_DAMAGE, ecs, SLOT(on_deal_damage)); }

void sys_heath_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Invulnerable* invulnerables;

  ecs_data(ecs, INVULNERABLE, &entities, (void**)&invulnerables, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--invulnerables[i].remaining == 0)
      ecs_rmv(ecs, entities[i], INVULNERABLE);
  }
}
