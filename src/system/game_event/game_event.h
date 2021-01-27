#ifndef GAME_EVENT_H
#define GAME_EVENT_H

typedef enum 
{
  GAME_EVENT_GET_WEAPON,
  GAME_EVENT_KILL_MONSTER,
} GameEventCode;

void game_event_init(void);
void game_event_fini(void);
void notify_game_event_finished(GameEventCode event_code);

#endif //GAME_EVENT_H
