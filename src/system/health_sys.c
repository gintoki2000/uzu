#include "health_sys.h"

#include "entity_factory.h"
#include "event_messaging_sys.h"
#include <components.h>

extern Ecs* g_ecs;

static void on_deal_damage(void* arg, const MSG_DealDamage* event)
{
  (void)arg;
  Health*       health;
  Motion*       motion;
  InputBlocker* input_blocker;
  if ((health = ecs_get(g_ecs, event->receiver, HEALTH)) != NULL &&
      !ecs_has(g_ecs, event->receiver, INVULNERABLE))
  {
    health->hit_points -= event->damage;

    INFO("e" ECS_ENT_FMT_PATTERN " take %d damage\n",
         ECS_ENT_FMT_VARS(event->receiver),
         event->damage);
    if (health->hit_points <= 0)
    {
      health->hit_points = 0;
      ems_broadcast(MSG_ENTITY_DIED, &(MSG_EntityDied){ event->receiver });
      ecs_add(g_ecs, event->receiver, DESTROYED_TAG);
    }
    else
    {
      Invulnerable* invulnerable;
      ems_broadcast(MSG_GET_DAMAGED,
                    &(MSG_GetDamaged){
                        .dealer  = event->dealer,
                        .damagee = event->receiver,
                        .damage  = event->damage,
                        .type    = event->type,
                    });
      invulnerable            = ecs_add(g_ecs, event->receiver, INVULNERABLE);
      invulnerable->remaining = 10;

      if (event->impact && (motion = ecs_get(g_ecs, event->receiver, MOTION)))
      {
        motion->acc.x += event->force.x;
        motion->acc.y += event->force.y;
        motion->vz = event->zforce;
        if (!ecs_has(g_ecs, event->receiver, INPUT_BLOCKER))
        {
          input_blocker            = ecs_add(g_ecs, event->receiver, INPUT_BLOCKER);
          input_blocker->remaining = event->impact_time;
        }
      }
    }
  }
}

static ecs_entity_t (*const _effect_fn_tbl[])(Ecs*, Vec2) = {
  make_blood_loss_particle, make_blood_loss_particle, make_fire_hit_effect,
  make_blood_loss_particle, make_ice_hit_effect,
};

static COLOR _color_tbl[] = {
  { 0xff, 0x00, 0x00, 0xff }, { 0xff, 0x00, 0x00, 0xff }, {0xfc, 0xad, 0x03, 0xff } ,
  { 0xff, 0x00, 0x00, 0xff }, { 0x03, 0xba, 0xfc, 0xff },
};

// TODO: tạo một hệ thống react lại các  event và sinh ra các effect
static void on_get_damaged(void* arg, const MSG_GetDamaged* event)
{
  (void)arg;
  Transform* transform;
  HitBox*    hitbox;
  Vec2       particle_position;
  Vec2       damagee_topleft;

  if ((transform = ecs_get(g_ecs, event->damagee, TRANSFORM)))
  {
    particle_position.x = transform->position.x;
    particle_position.y = transform->position.y - 30.f;
    make_damage_indicator_particle(g_ecs,
									particle_position,
									_color_tbl[event->type],
									event->damage);
  }

  if (transform && (hitbox = ecs_get(g_ecs, event->damagee, HITBOX)))
  {
    damagee_topleft.x = transform->position.x - hitbox->anchor.x;
    damagee_topleft.y = transform->position.y - hitbox->anchor.y;

    particle_position.x = rand() % ((int)hitbox->size.x) + damagee_topleft.x;
    particle_position.y = rand() % ((int)hitbox->size.y) + damagee_topleft.y;

    _effect_fn_tbl[event->type](g_ecs, particle_position);
  }
}

void health_system_init()
{
  ems_connect(MSG_DEAL_DAMAGE, NULL, on_deal_damage);
  ems_connect(MSG_GET_DAMAGED, NULL, on_get_damaged);
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
