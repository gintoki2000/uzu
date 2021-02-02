#include "system/self_destruction.h"

#include "../../include/entity_utils.h"
#include "components.h"
#include "entity_factory.h"
#include "system/event_messaging_sys.h"
#include "resources.h"

extern Ecs* g_ecs;

#define TIMER_TICK(__timer) ((__timer) > 0 && --(__timer) == 0)

#define EMIT_PARTICLE_DISTANCE 100

void self_destruction_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  SelfDestruction* self_destruction_array;

  Vec2  position;
  Vec2  target_position;
  float distance;
  Vec2  rposition;
  Vec2  vdir;
  Vec2  force;

  ecs_raw(g_ecs, SELF_DESTRUCTION, &entities, (void**)&self_destruction_array, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    position        = get_entity_position(g_ecs, entities[i]);
    target_position = get_entity_position(g_ecs, get_player(g_ecs));
    rposition       = vec2_sub(position, target_position);
    distance        = vec2_mag(rposition);
    vdir            = vec2_unit_vec(rposition);

    /*
    if (distance > EMIT_PARTICLE_DISTANCE)
      self_destruction_array[i].emiting_timer = self_destruction_array[i].emiting_interval;

    if (distance < EMIT_PARTICLE_DISTANCE && TIMER_TICK(self_destruction_array[i].emiting_timer))
    {
      self_destruction_array[i].emiting_timer = self_destruction_array[i].emiting_interval;
      make_fire_bust_effect(g_ecs, position);
    }
    */
    force = vec2_mul(vdir, -150.f);
    if (distance < self_destruction_array[i].range)
    {
      ems_broadcast(MSG_DEAL_DAMAGE,
                    &(MSG_DealDamage){ .dealer      = entities[i],
                                       .damage      = 15,
                                       .type        = DAMAGE_TYPE_FIRE,
                                       .receiver    = get_player(g_ecs),
                                       .impact      = TRUE,
                                       .force       = force,
                                       .impact_time = 60 });
      ecs_add(g_ecs, entities[i], DESTROYED_TAG);
  
      Mix_PlayChannel(-1, get_sfx(SFX_EXPOLOSION), 0);
    }
  }
}
