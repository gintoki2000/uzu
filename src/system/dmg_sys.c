#include "dmg_sys.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <resources.h>
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

static const Vec2 k_trap_impact_force = { 100.f, 0.f };

#define k_trap_impact_force_z 60.f

static void on_hit_trap(void* arg, const MSG_EntityHitTrap* event)
{
  (void)arg;
  Transform* transform = ecs_get(g_ecs, event->entity, TRANSFORM);
  Vec2       force     = vec2_mul(k_trap_impact_force, -transform->hdir);
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .damage      = 1,
                    .dealer      = ECS_NULL_ENT,
                    .receiver    = event->entity,
                    .type        = DAMAGE_TYPE_THUST,
                    .impact      = TRUE,
                    .impact_time = 10,
                    .force       = force,
                    .zforce      = k_trap_impact_force_z,
                });
}

static void on_projectile_hit(void* arg, const MSG_ProjectileHit* event)
{
  (void)arg;
  ProjectileAttributes* attributes;
  if ((attributes = ecs_get(g_ecs, event->projectile, PROJECTILE_ATTRIBUTES)) &&
      ecs_has(g_ecs, event->entity, HEALTH))
  {
    ems_broadcast(MSG_DEAL_DAMAGE,
                  &(MSG_DealDamage){
                      .damage      = attributes->damage,
                      .dealer      = ECS_NULL_ENT,
                      .receiver    = event->entity,
                      .type        = attributes->damage_type,
                      .impact      = attributes->impact,
                      .force       = attributes->impact_force,
                      .impact_time = attributes->impact_time,
                      .zforce      = attributes->impact_force_z,
                  });
    if (attributes->destroy_when_hit)
    {
      ecs_add(g_ecs, event->projectile, DESTROYED_TAG);
    }
  }
}

void damage_system_init()
{
  ems_connect(MSG_HIT_TRAP, NULL, on_hit_trap);
  ems_connect(MSG_PROJECTILE_HIT, NULL, on_projectile_hit);
}
