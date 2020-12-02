#include "generic_bow_system.h"
#include <components.h>

void GenericBowSystem(Ecs* ecs)
{
  ecs_entity_t* bows;
  ecs_size_t    cnt;

  GenericBow*     g_bows;
  WeaponCmdInput* cmd;
  Transform*      transform;

  ecs_data(ecs, GENERIC_BOW, &bows, &g_bows, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, bows[i], WP_CMD_INPUT);
    transform = ecs_get(ecs, bows[i], TRANSFORM);

    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      INFO("bow regular action\n");
      cmd->action = WEAPON_ACTION_NONE;
    }
    if (cmd->action == WEAPON_ACTION_SPECIAL_ATK)
    {
      INFO("bow special action\n");
      cmd->action = WEAPON_ACTION_NONE;
    }
  }
}
