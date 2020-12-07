#include "weapon_dealing_damage_system.h"
#include "mediator.h"
#include <components.h>
#include <resources.h>
#include <toolbox/toolbox.h>

static void on_weapon_hit(Ecs* ecs, const SysEvt_WeaponHit* event)
{
  DamageOutput* damage_output;
  if ((damage_output = ecs_get(ecs, event->weapon, DAMAGE_OUTPUT)) != NULL)
  {
    if (damage_output->atk > 0)
    {
      mediator_emit(SIG_DEAL_DAMAGE,
                    &(SysEvt_DealDamage){
                        .damage   = damage_output->atk,
                        .dealer   = event->weapon,
                        .receiver = event->entity,
                        .type     = damage_output->type,
                    });
    }
  }
}

void weapon_dealing_damage_system_init(Ecs* ecs)
{
  mediator_connect(SIG_WEAPON_HIT, ecs, SLOT(on_weapon_hit));
}
