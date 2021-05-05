#ifndef KEYBOARD_H
#define KEYBOARD_H
#include "toolbox/toolbox.h"
#include <SDL2/SDL.h>

enum Button
{
  BUTTON_INTERACT,
  BUTTON_CANCEL,
  BUTTON_JUMP,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_OPEN_MENU,
  BUTTON_OPEN_INVENTORY,
  NUM_BUTTONS
};

#define BUTTON_MASK(X) (1 << (X))

typedef void (*key_handle_fn_t)(void);

typedef struct InputState
{
  void (*fn)(void*);
  void* data;
} InputState;

#define INPUT_STATE_INST_1(__fn)                                                                   \
  (InputState)                                                                                     \
  {                                                                                                \
    .fn = (__fn), .data = NULL                                                                     \
  }

#define INPUT_STATE_INST_2(__fn, __data)                                                           \
  (InputState)                                                                                     \
  {                                                                                                \
    .fn = (__fn), .data = (__data)                                                                 \
  }

void         input_init(void);
void         input_update(void);
void         input_push_state(InputState input_state);
void         input_pop_state(void);
BOOL         button_pressed(u16 button);
BOOL         button_just_pressed(u16 button);
SDL_Scancode button_to_scancode(u16 button);
void         update_mouse_state(void);
BOOL         mouse_button_just_pressed(u16 button);
BOOL         mouse_button_pressed(u16 button);
BOOL         mouse_button_repeated(u16 button);
BOOL         key_pressed(SDL_Scancode scancode);
BOOL         key_just_pressed(SDL_Scancode scancode);
BOOL         key_released(SDL_Scancode scancode);
BOOL         key_just_released(SDL_Scancode scancode);
#endif
