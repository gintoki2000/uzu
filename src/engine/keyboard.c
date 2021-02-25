#include "engine/keyboard.h"
#include "SDL_log.h"
#include "toolbox/toolbox.h"
#include <string.h>

static const int _key_to_scancode_tbl[NUM_BUTTONS] = {
  [BUTTON_UP]             = SDL_SCANCODE_W,
  [BUTTON_DOWN]           = SDL_SCANCODE_S,
  [BUTTON_LEFT]           = SDL_SCANCODE_A,
  [BUTTON_RIGHT]          = SDL_SCANCODE_D,
  [BUTTON_INTERACT]       = SDL_SCANCODE_E,
  [BUTTON_CANCEL]         = SDL_SCANCODE_Q,
  [BUTTON_OPEN_MENU]      = SDL_SCANCODE_ESCAPE,
  [BUTTON_JUMP]           = SDL_SCANCODE_SPACE,
  [BUTTON_OPEN_INVENTORY] = SDL_SCANCODE_I,
};

static u32 _curr_mouse_state;
static u32 _prev_mouse_state;
static u32 _curr_button_state;
static u32 _prev_button_state;

#define KEY_HANDLE_FN_STACK_MAX 10

static InputState _state_stack[KEY_HANDLE_FN_STACK_MAX];
static u32        _stack_cnt;

void input_init()
{
}

static void update_button_state(void)
{
  const Uint8* keyboard_state = SDL_GetKeyboardState(NULL);

  _prev_button_state = _curr_button_state;
  _curr_button_state = 0;

  for (int i = 0; i < NUM_BUTTONS; ++i)
  {
    _curr_button_state |= (keyboard_state[_key_to_scancode_tbl[i]] & 1) << i;
  }
}

void input_update()
{
  update_button_state();
  update_mouse_state();

  InputState current_state;
  if (_stack_cnt > 0)
  {
    current_state = _state_stack[_stack_cnt - 1];
    current_state.fn(current_state.data);
  }
}

SDL_bool button_pressed(u16 button)
{
  return _curr_button_state & BUTTON_MASK(button);
}

SDL_bool button_just_pressed(u16 button)
{
  return (_curr_button_state & BUTTON_MASK(button)) &&
         !(_prev_button_state & (BUTTON_MASK(button)));
}

void input_push_state(InputState input_state)
{
  if (_stack_cnt < KEY_HANDLE_FN_STACK_MAX)
    _state_stack[_stack_cnt++] = input_state;
}

void input_pop_state()
{
  if (_stack_cnt > 0)
    _stack_cnt--;
}

SDL_Scancode button_to_scancode(u16 button)
{
  ASSERT(button < NUM_BUTTONS);
  return _key_to_scancode_tbl[button];
}

void update_mouse_state()
{
  _prev_mouse_state = _curr_mouse_state;
  _curr_mouse_state = SDL_GetMouseState(NULL, NULL);
}

BOOL mouse_button_just_pressed(u16 mouse_button)
{
  return !(_prev_mouse_state & SDL_BUTTON(mouse_button)) &&
         (_curr_mouse_state & SDL_BUTTON(mouse_button));
}

BOOL mouse_button_pressed(u16 mouse_button)
{
  return _curr_mouse_state & SDL_BUTTON(mouse_button);
}
