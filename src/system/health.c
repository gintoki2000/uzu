#include "system/game_logic.h"

#include "components.h"
#include "entity_factory.h"
#include "system/event_messaging_sys.h"

extern ecs_Registry* gRegistry;

static void on_inflict_damage(SDL_UNUSED void* arg, const InflictDamageMsg* msg)
{
  Health* health;
  Motion* motion;
  if ((health = ecs_get(gRegistry, msg->receiver, HEALTH)) != NULL &&
      !ecs_has(gRegistry, msg->receiver, INVULNERABLE))
  {
    //TODO: su dung them he thong chi so defense
    health->current -= min(msg->damage, health->current);
    ems_broadcast(MSG_GET_DAMAGED,
                  &(GetDamagedMsg){
                      .dealer  = msg->dealer,
                      .damagee = msg->receiver,
                      .damage  = msg->damage,
                      .type    = msg->type,
                  });
    ecs_set(gRegistry, msg->receiver, INVULNERABLE, &(Invulnerable){ msg->impactTime });

    if (msg->dealer != ECS_NULL_ENT)
      ecs_set(gRegistry, msg->receiver, ATTACKER, &(Attacker){ msg->dealer });

    if (msg->impactTime > 0 && (motion = ecs_get(gRegistry, msg->receiver, MOTION)))
    {
      motion->acc.x += msg->force.x;
      motion->acc.y += msg->force.y;
      motion->vz = msg->zforce;
      if (!ecs_has(gRegistry, msg->receiver, STAGGER))
        ecs_set(gRegistry, msg->receiver, STAGGER, &(Stagger){ msg->impactTime });
    }
    if (health->current <= 0)
    {
      health->current = 0;
      ems_broadcast(MSG_ENTITY_DIED, &(EntityDiedMsg){ msg->receiver });
      ecs_add(gRegistry, msg->receiver, DESTROYED_TAG);
    }
  }
}

void health_system_init()
{
  ems_connect(MSG_DEAL_DAMAGE, CALLBACK_2(on_inflict_damage));
}

void invulnerable_timer_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Invulnerable* invulnerables;

  ecs_raw(gRegistry, INVULNERABLE, &entities, (void**)&invulnerables, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (invulnerables[i].duration > 0 && --invulnerables[i].duration == 0)
      ecs_rmv(gRegistry, entities[i], INVULNERABLE);
  }
}
