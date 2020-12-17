#include <engine/keyboard.h>
#include <string.h>

static const int KEY_TO_SANCODE[] = {
  [KEY_A] = SDL_SCANCODE_A,
  [KEY_B] = SDL_SCANCODE_S,
  [KEY_UP] = SDL_SCANCODE_UP,
  [KEY_DOWN] = SDL_SCANCODE_DOWN,
  [KEY_LEFT] = SDL_SCANCODE_LEFT,
  [KEY_RIGHT] = SDL_SCANCODE_RIGHT
};

static int _curr_state[NUM_KEYS];
static int _prev_state[NUM_KEYS];

void keybroad_init()
{
  memset(_curr_state, 0, NUM_KEYS * sizeof(int));
  memset(_prev_state, 0, NUM_KEYS * sizeof(int));
}

void keybroad_update()
{
  const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
  memcpy(_prev_state, _curr_state, NUM_KEYS * sizeof(int));
  for (int i = 0; i < NUM_KEYS; ++i) {
    _curr_state[i] = keyboard_state[KEY_TO_SANCODE[i]];
  }
}

SDL_bool key_pressed(Key k)
{
  return _curr_state[k];
}

SDL_bool key_just_pressed(Key k)
{
  return _curr_state[k] && !_prev_state[k];
}
