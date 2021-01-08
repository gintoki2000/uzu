#include "game_event.h"
#include "../event_messaging_sys.h"
extern void GE_get_weapon_init();
extern void GE_kill_monsters_init();

void game_event_init()
{
  GE_get_weapon_init();
  GE_kill_monsters_init();
}

void notify_game_event_is_finished(GameEventCode event_code)
{
	ems_broadcast(MSG_EVENT_FINISHED, &(MSG_EventFinished){ event_code });
}
