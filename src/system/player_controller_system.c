#include <SDL2/SDL.h>
#include <components.h>
#include <system/player_controller_system.h>

void PlayerControllerSystem(Ecs* ecs)
{
  ecs_entity_t* player;
  CommandInput* cmd;
  const Uint8*  key_state;

  ecs_data(ecs, PLAYER_TAG, &player, NULL, NULL);

  cmd = ecs_get(ecs, player[0], COMMAND_INPUT);
  key_state = SDL_GetKeyboardState(NULL);

  if (key_state[SDL_SCANCODE_UP])
    cmd->desired_dir = UP;
  if (key_state[SDL_SCANCODE_DOWN])
    cmd->desired_dir = DOWN;
  if (key_state[SDL_SCANCODE_LEFT])
    cmd->desired_dir = LEFT;
  if (key_state[SDL_SCANCODE_RIGHT])
    cmd->desired_dir = RIGHT;
}
