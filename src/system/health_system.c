#include "health_system.h"
#include <components.h>

static Dispatcher* _dispatcher;

void health_system_init() { _dispatcher = dispatcher_new(NUM_HEALTH_SYSTEM_SIGS); }

void health_system_fini() { dispatcher_destroy(_dispatcher); }

void health_system_apply_damage(Ecs* ecs, ecs_entity_t entity, int damage)
{
  Heath* health;
  if ((health = ecs_get(ecs, entity, HEATH)) != NULL)
  {
    health->hit_points -= damage;
    if (health->hit_points <= 0)
    {
      EntityDiedEvent evt = (EntityDiedEvent){entity};
      dispatcher_emit(_dispatcher, SIG_ENTITY_DIED, &evt);
      ecs_destroy(ecs, entity);
    }
  }
}
