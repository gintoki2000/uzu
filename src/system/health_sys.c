#include "health_sys.h"

#include "entity_factory.h"
#include "mediator.h"
#include <components.h>

extern Ecs* g_ecs;

static void on_deal_damage(void* arg, const SysEvt_DealDamage* event)
{
  (void)arg;
  Heath* health;
  if ((health = ecs_get(g_ecs, event->receiver, HEATH)) != NULL &&
      !ecs_has(g_ecs, event->receiver, INVULNERABLE))
  {
    health->hit_points -= event->damage;

    INFO("e " ECS_ENT_FMT_PATTERN "take %d damage\n",
         ECS_ENT_FMT_VARS(event->receiver),
         event->damage);
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

static void on_get_damaged(void* arg, const SysEvt_GetDamaged* event)
{
  (void)arg;
  Transform* transform;

  if ((transform = ecs_get(g_ecs, event->damagee, TRANSFORM)))
  {
    make_make_damage_indicator_particle(g_ecs,
                                        (Vec2){ transform->pos.x, transform->pos.y - 30.f },
                                        event->damage);
  }
}

void health_system_init()
{
  mediator_connect(SYS_SIG_DEAL_DAMAGE, NULL, SLOT(on_deal_damage));
  mediator_connect(SYS_SIG_GET_DAMAGED, NULL, SLOT(on_get_damaged));
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
