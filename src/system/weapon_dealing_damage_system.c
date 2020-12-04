#include "weapon_dealing_damage_system.h"
#include "mediator.h"
#include <components.h>
#include <toolbox/toolbox.h>

static void on_weapon_hit(Ecs* ecs, const SysEvt_WeaponHit* event)
{
  DamageOutput* damage_output;
  damage_output = ecs_get(ecs, event->weapon, DAMAGE_OUTPUT);
  ASSERT(damage_output != NULL);
  if (damage_output->atk > 0)
  {
    mediator_emit(SIG_DEAL_DAMAGE,
                  &(SysEvt_DealDamage){
                      .damage   = damage_output->atk,
                      .dealer   = event->weapon,
                      .receiver = event->entity,
                  });
  }
}

void weapon_dealing_damage_system_init(Ecs* ecs)
{
  mediator_connect(SIG_WEAPON_HIT, ecs, SLOT(on_weapon_hit));
}
