#include <components.h>
#include <system/generic_axe_system.h>

#define INTERVAL 1
#define STEP 7

static void process(Ecs* ecs, GenericAxe* gaxe, WeaponAction* cmd, Visual* vs, Transform* tx)
{
  (void)ecs;

  switch (gaxe->state)
  {
  case AXE_STATE_IDLE:
  {
    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      gaxe->state = AXE_STATE_REGULAR_ATK;
      gaxe->step = 1;
      gaxe->timer = 0;
      tx->rot = 20.0 * FLIP_TO_SIGN(vs->flip);
    }
    else if (cmd->action == WEAPON_ACTION_SPECIAL_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      // TODO: heavy attack
    }
  }
  break;
  case AXE_STATE_REGULAR_ATK:
  {
    gaxe->timer++;
    if (gaxe->timer == INTERVAL)
    {
      gaxe->timer = 0;
      gaxe->step++;
      if (gaxe->step == STEP)
      {
        gaxe->state = AXE_STATE_IDLE;
        tx->rot = 0.0;
      }
      else
      {
        tx->rot = gaxe->step * 20.0 * FLIP_TO_SIGN(vs->flip);
      }
    }
  }
  }
}

void GenericAxeSystem(Ecs* ecs)
{
  ecs_entity_t*   ett;
  WeaponAction* cmd;
  GenericAxe*     gaxe;
  Visual*         vs;
  Transform*      tx;
  ecs_size_t      cnt;

  ecs_data(ecs, GENERIC_AXE, &ett, (void**)&gaxe, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, ett[i], WEAPON_ACTION);
    vs = ecs_get(ecs, ett[i], VISUAL);
    tx = ecs_get(ecs, ett[i], TRANSFORM);
    process(ecs, &gaxe[i], cmd, vs, tx);
  }
}
