#include "game_event.h"
#include "system/event_messaging_sys.h"

void brian_first_encounter_init();
void brian_first_encounter_fini();
void game_event_init()
{
  brian_first_encounter_init();
}

void game_event_fini()
{
  brian_first_encounter_fini();
}
