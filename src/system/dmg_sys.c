#include "dmg_sys.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <resources.h>
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

static void on_weapon_hit(void* arg, const MSG_WeaponHit* event)
{
  (void)arg;
  DamageOutput* damage_output;
  if ((damage_output = ecs_get(g_ecs, event->weapon, DAMAGE_OUTPUT)) != NULL)
  {
    if (damage_output->atk > 0)
    {
      ems_broadcast(MSG_DEAL_DAMAGE,
                    &(MSG_DealDamage){
                        .damage      = damage_output->atk,
                        .dealer      = event->weapon,
                        .receiver    = event->entity,
                        .type        = damage_output->type,
                        .impact      = damage_output->impact,
                        .impact_time = 10,
                        .force       = damage_output->force,
                    });
    }
  }
}

static void on_hit_trap(void* arg, const MSG_EntityHitTrap* event)
{
  (void)arg;
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){
                    .damage   = 1,
                    .dealer   = ECS_NULL_ENT,
                    .receiver = event->entity,
                    .type     = DAMAGE_TYPE_THUST,
                });
}

void damage_system_init()
{
  ems_connect(MSG_WEAPON_HIT, NULL, (on_weapon_hit));
  ems_connect(MSG_HIT_TRAP, NULL, (on_hit_trap));
}
