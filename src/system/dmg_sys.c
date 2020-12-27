#include "mediator.h"
#include <components.h>
#include <resources.h>
#include <toolbox/toolbox.h>
#include "dmg_sys.h"

extern Ecs* g_ecs;

static void on_weapon_hit(void* arg, const SysEvt_WeaponHit* event)
{
  DamageOutput* damage_output;
  if ((damage_output = ecs_get(g_ecs, event->weapon, DAMAGE_OUTPUT)) != NULL)
  {
    if (damage_output->atk > 0)
    {
      mediator_broadcast(SYS_SIG_DEAL_DAMAGE,
                    &(SysEvt_DealDamage){
                        .damage   = damage_output->atk,
                        .dealer   = event->weapon,
                        .receiver = event->entity,
                        .type     = damage_output->type,
                    });
    }
  }
}

static void on_hit_trap(void* arg, const SysEvt_EntityHitTrap* event)
{
	mediator_broadcast(SYS_SIG_DEAL_DAMAGE, &(SysEvt_DealDamage){
		.damage = 1,
		.dealer = ECS_NULL_ENT,
		.receiver = event->entity,
		.type = DAMAGE_TYPE_THUST,
	});
}

void damage_system_init()
{
  mediator_connect(SYS_SIG_WEAPON_HIT, NULL, SLOT(on_weapon_hit));
  mediator_connect(SYS_SIG_HIT_TRAP, NULL, SLOT(on_hit_trap));
}
