#include <components.h>
#include <system/generic_axe_system.h>

#define INTERVAL 1
#define STEP 7
#define FLIP_TO_SIGN(f) (f == SDL_FLIP_NONE ? 1 : -1)

static void process(Ecs* ecs, GenericAxe* ax, WeaponCmdInput* cmd, Visual* vs)
{
  (void)ecs;

  switch (ax->state)
  {
  case AXE_STATE_IDLE:
  {
    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      ax->state = AXE_STATE_REGULAR_ATK;
      ax->step = 1;
      ax->timer = 0;
      vs->rot = 20.0 * FLIP_TO_SIGN(vs->flip);
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
    ax->timer++;
    if (ax->timer == INTERVAL)
    {
      ax->timer = 0;
      ax->step++;
      if (ax->step == STEP)
      {
        ax->state = AXE_STATE_IDLE;
        vs->rot = 0.0;
      }
      else
      {
        vs->rot = ax->step * 20.0 * FLIP_TO_SIGN(vs->flip);
      }
    }
  }
  }
}

void GenericAxeSystem(Ecs* ecs)
{
  ecs_entity_t*   ett;
  WeaponCmdInput* cmd;
  GenericAxe*     ax;
  Visual*         vs;
  ecs_size_t      cnt;

  ecs_data(ecs, GENERIC_AXE, &ett, (void**)&ax, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, ett[i], WP_CMD_INPUT);
    vs = ecs_get(ecs, ett[i], VISUAL);
    process(ecs, &ax[i], cmd, vs);
  }
}
