#include "ecs/ecs.h"
#include <SDL2/SDL.h>
#include <components.h>
#include <engine/keyboard.h>
#include <system/player_controller_system.h>

void PlayerControllerSystem(Ecs* ecs)
{
  ecs_entity_t* player;
  CmdInput*     cmd;
  Equipment*    eqm;
  Visual*       vs;
  Animator*     ani;
  SDL_bool      moved;

  ecs_data(ecs, PLAYER_TAG, &player, NULL, NULL);

  cmd = ecs_get(ecs, player[0], COMMAND_INPUT);
  vs = ecs_get(ecs, player[0], VISUAL);
  ani = ecs_get(ecs, player[0], ANIMATOR);

  moved = SDL_FALSE;

  if (key_pressed(KEY_UP))
  {
    cmd->desired_dir = UP;
    moved = SDL_TRUE;
  }
  if (key_pressed(KEY_DOWN))
  {
    cmd->desired_dir = DOWN;
    moved = SDL_TRUE;
  }
  if (key_pressed(KEY_LEFT))
  {
    cmd->desired_dir = LEFT;
    vs->flip = SDL_FLIP_HORIZONTAL;
    moved = SDL_TRUE;
  }
  if (key_pressed(KEY_RIGHT))
  {
    cmd->desired_dir = RIGHT;
    vs->flip = SDL_FLIP_NONE;
    moved = SDL_TRUE;
  }

  if (moved && ani->current_anim != ANIM_STATE_RUN)
  {
    ani->current_anim = ANIM_STATE_RUN;
    ani->elapsed = 0;
  }
  else if (!moved && ani->current_anim != ANIM_STATE_IDLE)
  {
    ani->current_anim = ANIM_STATE_IDLE;
    ani->elapsed = 0;
  }

  if (key_just_pressed(KEY_A))
  {
    WeaponCmdInput* wcmd;

    eqm = ecs_get(ecs, player[0], EQUIPMENT);

    wcmd = ecs_get(ecs, eqm->rhand, WP_CMD_INPUT);
    wcmd->action = WEAPON_ACTION_REGULAR_ATK;
  }
}
