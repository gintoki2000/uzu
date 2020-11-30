#include <components.h>
#include <stdio.h>
#include <system/sword_system.h>

#define INTERVAL 1
#define STEP 7
#define FLIP_TO_SIGN(f) (f == SDL_FLIP_NONE ? 1 : -1)

static void process(Ecs* ecs, GenericSword* sw, WeaponCmdInput* cmd, Visual* vs)
{
  (void)ecs;

  switch (sw->state)
  {
  case SWORD_STATE_IDLE:
  {
    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      sw->state = SWORD_STATE_REGULAR_ATK;
      sw->step = 1;
      sw->timer = 0;
      vs->rot = 20.0 * FLIP_TO_SIGN(vs->flip);
    }
  }
  break;
  case SWORD_STATE_REGULAR_ATK:
  {
    sw->timer++;
    if (sw->timer == INTERVAL)
    {
      sw->timer = 0;
      sw->step++;
      if (sw->step == STEP)
      {
        sw->state = SWORD_STATE_IDLE;
        vs->rot = 0.0;
      }
      else
      {
        vs->rot = sw->step * 20.0 * FLIP_TO_SIGN(vs->flip);
      }
    }
  }
  }
}

void GenericSwordSystem(Ecs* ecs)
{
  ecs_entity_t*   ett;
  WeaponCmdInput* cmd;
  GenericSword*   sw;
  Visual*         vs;
  ecs_size_t      cnt;

  ecs_data(ecs, GENERIC_SWORD, &ett, (void**)&sw, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, ett[i], WP_CMD_INPUT);
    vs = ecs_get(ecs, ett[i], VISUAL);
    process(ecs, &sw[i], cmd, vs);
  }
}
