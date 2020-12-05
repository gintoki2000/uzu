#include "ecs/ecs.h"
#include <SDL2/SDL.h>
#include <components.h>
#include <engine/keyboard.h>
#include <system/player_controller_system.h>

void PlayerControllerSystem(Ecs* ecs)
{
  ecs_entity_t*    player;
  CharacterAction* cmd;
  Equipment*       eqm;
  Visual*          vs;

  ecs_data(ecs, PLAYER_TAG, &player, NULL, NULL);

  cmd = ecs_get(ecs, player[0], CHARACTER_ACTION);
  vs  = ecs_get(ecs, player[0], VISUAL);

  if (key_pressed(KEY_UP))
  {
    cmd->desired_dir = UP;
  }
  if (key_pressed(KEY_DOWN))
  {
    cmd->desired_dir = DOWN;
  }
  if (key_pressed(KEY_LEFT))
  {
    cmd->desired_dir = LEFT;
  }
  if (key_pressed(KEY_RIGHT))
  {
    cmd->desired_dir = RIGHT;
  }

  if (key_just_pressed(KEY_A))
  {
    WeaponAction* wcmd;

    eqm = ecs_get(ecs, player[0], EQUIPMENT);

    wcmd         = ecs_get(ecs, eqm->rhand, WEAPON_ACTION);
    wcmd->action = WEAPON_ACTION_REGULAR_ATK;
  }
}
