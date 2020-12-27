#include "health_sys.h"

#include "mediator.h"
#include <components.h>

extern Ecs* g_ecs;

static void on_deal_damage(void* arg, const SysEvt_DealDamage* event)
{
  Heath* health;
  if ((health = ecs_get(g_ecs, event->receiver, HEATH)) != NULL &&
      !ecs_has(g_ecs, event->receiver, INVULNERABLE))
  {
    health->hit_points -= event->damage;
    INFO("take %d damage\n", event->damage);
    if (health->hit_points <= 0)
    {
      health->hit_points = 0;
      mediator_broadcast(SYS_SIG_ENTITY_DIED, &(SysEvt_EntityDied){ event->receiver });
      ecs_add(g_ecs, event->receiver, TAG_TO_BE_DESTROYED);
    }
    else
    {
      Invulnerable* invulnerable;
      mediator_broadcast(SYS_SIG_GET_DAMAGED,
                    &(SysEvt_GetDamaged){
                        .dealer  = event->dealer,
                        .damagee = event->receiver,
                        .damage  = event->damage,
                        .type    = event->type,
                    });
      invulnerable            = ecs_add(g_ecs, event->receiver, INVULNERABLE);
      invulnerable->remaining = 10;
    }
  }
}


void health_system_init()
{
  mediator_connect(SYS_SIG_DEAL_DAMAGE, NULL, SLOT(on_deal_damage));
}

void health_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Invulnerable* invulnerables;

  ecs_raw(g_ecs, INVULNERABLE, &entities, (void**)&invulnerables, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (--invulnerables[i].remaining == 0)
      ecs_rmv(g_ecs, entities[i], INVULNERABLE);
  }
}
