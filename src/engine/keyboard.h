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
  NUM_KEYS
} Key;

void     keybroad_init();
void     keybroad_update();
SDL_bool key_pressed(Key k);
SDL_bool key_just_pressed(Key k);
#endif // KEYBOARD_H
