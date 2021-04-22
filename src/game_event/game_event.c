#include "game_event/game_event.h"
#include "system/event_messaging_sys.h"

extern void GE_begining_init(void);

void game_event_init()
{
  //GE_get_weapon_init();
  //GE_kill_monsters_init();
  GE_begining_init();
}

void game_event_fini()
{

}
