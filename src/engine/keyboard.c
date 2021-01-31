#include <engine/keyboard.h>
#include <string.h>
#include <toolbox/toolbox.h>

static const int _key_to_scancode_tbl[NUM_KEYS] = {
  [KEY_A] = SDL_SCANCODE_A,          [KEY_B] = SDL_SCANCODE_S,
  [KEY_UP] = SDL_SCANCODE_UP,        [KEY_DOWN] = SDL_SCANCODE_DOWN,
  [KEY_LEFT] = SDL_SCANCODE_LEFT,    [KEY_RIGHT] = SDL_SCANCODE_RIGHT,
  [KEY_SELECT] = SDL_SCANCODE_SPACE, [KEY_R] = SDL_SCANCODE_W,
  [KEY_L] = SDL_SCANCODE_Q,
};

static int _curr_state[NUM_KEYS];
static int _prev_state[NUM_KEYS];

#define KEY_HANDLE_FN_STACK_MAX 10

static key_handle_fn_t _key_handle_fn_stack[KEY_HANDLE_FN_STACK_MAX];
static u32             _key_handle_fn_stack_cnt;

void keybroad_init()
{
  memset(_curr_state, 0, NUM_KEYS * sizeof(int));
  memset(_prev_state, 0, NUM_KEYS * sizeof(int));
}

void keybroad_update()
{
  const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);
  memcpy(_prev_state, _curr_state, NUM_KEYS * sizeof(int));
  for (int i = 0; i < NUM_KEYS; ++i)
  {
    _curr_state[i] = keyboard_state[_key_to_scancode_tbl[i]];
  }

  if (_key_handle_fn_stack_cnt > 0)
    _key_handle_fn_stack[_key_handle_fn_stack_cnt - 1]();
}

SDL_bool key_pressed(Key k)
{
  return _curr_state[k];
}

SDL_bool key_just_pressed(Key k)
{
  return _curr_state[k] && !_prev_state[k];
}

void keybroad_push_state(key_handle_fn_t key_handle_fn)
{
  if (_key_handle_fn_stack_cnt < KEY_HANDLE_FN_STACK_MAX)
    _key_handle_fn_stack[_key_handle_fn_stack_cnt++] = key_handle_fn;
}

void keybroad_pop_state()
{
  if (_key_handle_fn_stack_cnt > 0)
    _key_handle_fn_stack_cnt--;
}
