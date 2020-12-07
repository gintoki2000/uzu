#include "sound_sytem.h"
#include "mediator.h"
#include <resources.h>
#include <components.h>

static void on_get_damage(Ecs* ecs, const SysEvt_GetDamaged* event)
{
  if (event->type == DAMAGE_TYPE_STRIKE)
  {
    Mix_PlayChannel(-1, get_sfx(SFX_SWORD_HIT), 0);
  };
}

void sound_system_init(Ecs *ecs)
{
  mediator_connect(SIG_GET_DAMAGED, ecs, SLOT(on_get_damage));
}
