#ifndef KEYBOARD_H
#define KEYBOARD_H
#include <SDL2/SDL.h>
typedef enum
{
  KEY_A,
  KEY_B,
  KEY_UP,
  KEY_DOWN,
  KEY_LEFT,
  KEY_RIGHT,
  KEY_SELECT,
  NUM_KEYS
} Key;

typedef void (*key_handle_fn_t)(void);

void     keybroad_init();
void     keybroad_update();
SDL_bool key_pressed(Key k);
SDL_bool key_just_pressed(Key k);
void     keybroad_push_state(key_handle_fn_t key_handle_fn);
void     keybroad_pop_state();

#endif
