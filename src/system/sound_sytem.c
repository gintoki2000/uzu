#include "sound_sytem.h"
#include "mediator.h"
#include <resources.h>
#include <components.h>

static void on_get_damage(Ecs* ecs, const SysEvt_GetDamaged* event)
{
}

void sound_system_init(Ecs *ecs)
{
  mediator_connect(SIG_GET_DAMAGED, ecs, SLOT(on_get_damage));
}
