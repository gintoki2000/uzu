#include "ecs/common.h"
#include "ecs/ecs.h"
#include <components.h>
#include <system/sync_eqm_system.h>

#define FLIP_TO_SIGN(f) (f == SDL_FLIP_NONE ? 1 : -1)

void SyncEqmSystem(Ecs* ecs)
{
  ecs_entity_t* ett;
  Equipment*    eqm;
  Transform*    tx;
  Visual*       vs;
  Transform*    eqmtx;
  Visual*       eqmvs;
  ecs_size_t    cnt;
  ecs_entity_t  eqme;

  ecs_data(ecs, EQUIPMENT, &ett, (void**)&eqm, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    eqme = eqm[i].rhand;

    tx = ecs_get(ecs, ett[i], TRANSFORM);
    vs = ecs_get(ecs, ett[i], VISUAL);

    eqmtx = ecs_get(ecs, eqme, TRANSFORM);
    eqmvs = ecs_get(ecs, eqme, VISUAL);

    eqmtx->x = tx->x + FLIP_TO_SIGN(vs->flip) * 6;
    eqmtx->y = tx->y + 6;

    eqmvs->flip = vs->flip;
  }
}
