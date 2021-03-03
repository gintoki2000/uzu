#include "system/game_logic.h"

#include "components.h"
#include "entity_factory.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

static void on_deal_damage(void* arg, const MSG_DealDamage* event)
{
  (void)arg;
  Health*    health;
  Motion*    motion;
  Paralyzed* paralyzed;
  if ((health = ecs_get(g_ecs, event->receiver, HEALTH)) != NULL &&
      !ecs_has(g_ecs, event->receiver, INVULNERABLE))
  {
    health->hit_points -= event->damage;
    Invulnerable* invulnerable;
    ems_broadcast(MSG_GET_DAMAGED,
                  &(MSG_GetDamaged){
                      .dealer  = event->dealer,
                      .damagee = event->receiver,
                      .damage  = event->damage,
                      .type    = event->type,
                  });
    invulnerable            = ecs_add(g_ecs, event->receiver, INVULNERABLE);
    invulnerable->remaining = event->impact_time;

    if (event->dealer != ECS_NULL_ENT)
    {
      ecs_add_or_set(g_ecs, event->receiver, ATTACKER, &(Attacker){ event->dealer });
    }

    if (event->impact_time > 0 && (motion = ecs_get(g_ecs, event->receiver, MOTION)))
    {
      motion->acc.x += event->force.x;
      motion->acc.y += event->force.y;
      motion->vz = event->zforce;
      if (!ecs_has(g_ecs, event->receiver, PARALYZED))
      {
        paralyzed            = ecs_add(g_ecs, event->receiver, PARALYZED);
        paralyzed->remaining = event->impact_time;
      }
    }
    if (health->hit_points <= 0)
    {
      health->hit_points = 0;
      ems_broadcast(MSG_ENTITY_DIED, &(MSG_EntityDied){ event->receiver });
      ecs_add(g_ecs, event->receiver, DESTROYED_TAG);
    }
  }
}

void health_system_init()
{
  ems_connect(MSG_DEAL_DAMAGE, NULL, on_deal_damage);
}

void health_system()
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
