#include "player_ctl_sys.h"

#include <components.h>
#include <ecs/ecs.h>
#include <engine/keyboard.h>
#include <utils.h>

void sys_player_controller_update(Ecs* ecs)
{

  ecs_entity_t player;
  Controller*  controller;

  player     = get_player(ecs);
  controller = ecs_get(ecs, player, CONTROLLER);


  controller->desired_vel = VEC2_ZERO;

  if (key_pressed(KEY_UP))
  {
    controller->desired_vel.y -= 1 * 100;
  }
  if (key_pressed(KEY_DOWN))
  {
    controller->desired_vel.y += 1 * 100;
  }
  if (key_pressed(KEY_LEFT))
  {
    controller->desired_vel.x -= 1 * 100;
  }
  if (key_pressed(KEY_RIGHT))
  {
    controller->desired_vel.x += 1 * 100;
  }

  if (key_just_pressed(KEY_A))
  {
    controller->action = CHARACTER_ACTION_REGULAR_ATK;
  }

  else if (key_just_pressed(KEY_B))
  {

    controller->action = CHARACTER_ACTION_SPECIAL_ATK;
  }
}
