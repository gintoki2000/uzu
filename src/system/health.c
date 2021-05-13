#include "system/game_logic.h"

#include "components.h"
#include "entity_factory.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

static void on_deal_damage(void* arg, const InflictDamageMsg* msg)
{
  (void)arg;
  Health* health;
  Motion* motion;
  if ((health = ecs_get(g_ecs, msg->receiver, HEALTH)) != NULL &&
      !ecs_has(g_ecs, msg->receiver, INVULNERABLE))
  {
    health->current -= min(msg->damage, health->current);
    ems_broadcast(MSG_GET_DAMAGED,
                  &(GetDamagedMsg){
                      .dealer  = msg->dealer,
                      .damagee = msg->receiver,
                      .damage  = msg->damage,
                      .type    = msg->type,
                  });
    ecs_set(g_ecs, msg->receiver, INVULNERABLE, &(Invulnerable){ msg->impact_time });

    if (msg->dealer != ECS_NULL_ENT)
      ecs_set(g_ecs, msg->receiver, ATTACKER, &(Attacker){ msg->dealer });

    if (msg->impact_time > 0 && (motion = ecs_get(g_ecs, msg->receiver, MOTION)))
    {
      motion->acc.x += msg->force.x;
      motion->acc.y += msg->force.y;
      motion->vz = msg->zforce;
      if (!ecs_has(g_ecs, msg->receiver, STAGGER))
        ecs_set(g_ecs, msg->receiver, STAGGER, &(Stagger){ msg->impact_time });
    }
    if (health->current <= 0)
    {
      health->current = 0;
      ems_broadcast(MSG_ENTITY_DIED, &(EntityDiedMsg){ msg->receiver });
      ecs_add(g_ecs, msg->receiver, DESTROYED_TAG);
    }
  }
}

void health_system_init()
{
  ems_connect(MSG_DEAL_DAMAGE, CALLBACK_2(on_deal_damage));
}

void health_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Invulnerable* invulnerables;

  ecs_raw(g_ecs, INVULNERABLE, &entities, (void**)&invulnerables, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (invulnerables[i].duration > 0 && --invulnerables[i].duration == 0)
      ecs_rmv(g_ecs, entities[i], INVULNERABLE);
  }
}
