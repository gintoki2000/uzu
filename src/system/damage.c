#include "camera_shaker.h"
#include "components.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include "toolbox/toolbox.h"
#include "entity_utils.h"

extern ecs_Registry* gRegistry;

static const Vec2 _trapImpactForce = { 100.f, 0.f };

#define TRAP_IMPACT_FORCE_Z 60.f

static void on_hit_trap(SDL_UNUSED void* arg, const EntityHitTrapMsg* event)
{
  const AimDirection*    aimDirection = ecs_get(gRegistry, event->entity, AIM_DIRECTION);
  Vec2                   force = vec2_mul(_trapImpactForce, signf(aimDirection->value.x));
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(InflictDamageMsg){ .damage     = 1,
                                     .dealer     = ECS_NULL_ENT,
                                     .receiver   = event->entity,
                                     .type       = DAMAGE_TYPE_THUST,
                                     .impactTime = 10,
                                     .force      = force,
                                     .zforce     = TRAP_IMPACT_FORCE_Z });
}

static void on_projectile_hit(SDL_UNUSED void* arg, const ProjectileHitMsg* event)
{
  ProjectileAttributes* attributes;
  if ((attributes = ecs_get(gRegistry, event->projectile, PROJECTILE_ATTRIBUTES)) != NULL)
  {
    ems_broadcast(MSG_DEAL_DAMAGE,
                  &(InflictDamageMsg){
                      .damage     = attributes->damage,
                      .dealer     = attributes->shooter,
                      .receiver   = event->entity,
                      .type       = attributes->damageType,
                      .force      = attributes->impactForce,
                      .impactTime = attributes->impactTime,
                      .zforce     = attributes->impactForceZ,
                  });
    ett_apply_status_effect(gRegistry, event->entity, STATUS_EFFECT_FREEZED, 240);
    begin_sake_camera(4, 6);
    if (attributes->destroyWhenHit)
    {
      ecs_add(gRegistry, event->projectile, DESTROYED_TAG);
    }
  }
}

void damage_system_init()
{
  ems_connect(MSG_HIT_TRAP, CALLBACK_2(on_hit_trap));
  ems_connect(MSG_PROJECTILE_HIT, CALLBACK_2(on_projectile_hit));
}
