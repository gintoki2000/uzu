#include "game_event.h"
#include "system/event_messaging_sys.h"

void brian_first_encounter_init();
void brian_first_encounter_fini();
void brian_rescue_init();
void brian_rescue_fini();
void game_event_init()
{
  brian_first_encounter_init();
  brian_rescue_init();
}

void game_event_fini()
{
  brian_first_encounter_fini();
  brian_rescue_fini();
}
