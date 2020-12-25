#ifndef DUNGEON_H
#define DUNGEON_H
#include <toolbox/toolbox.h>
typedef void (*scr_on_load_fn_t)(void);
typedef void (*scr_on_unload_t)(void);
typedef void (*scr_on_update_t)(void);
typedef void (*scr_on_evt_fn_t)(const SDL_Event*);

typedef struct Screen
{
  scr_on_load_fn_t on_load;
  scr_on_unload_t  on_unload;
  scr_on_evt_fn_t  on_event;
  scr_on_update_t  on_update;
} Screen;

void set_scr(const Screen* newscr);
#endif // DUNGEON_H
