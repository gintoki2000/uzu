#include "game_event.h"

extern void GE_get_weapon_init();
extern void GE_kill_monsters_init();

void game_event_init()
{
  GE_get_weapon_init();
  GE_kill_monsters_init();
}
