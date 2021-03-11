#ifndef GAME_EVENT_H
#define GAME_EVENT_H

typedef enum
{
  GAME_EVENT_GET_WEAPON,
  GAME_EVENT_KILL_MONSTER,
} GameEventCode;

#define SAVE_AND_LOAD_FN()                                                                         \
  static void init_default_data(void);                                                             \
  static void write_data(void)                                                                     \
  {                                                                                                \
    SDL_RWops* rw;                                                                                 \
    if ((rw = SDL_RWFromFile(SAV_FILE_NAME, "w")) != NULL)                                         \
    {                                                                                              \
      SDL_RWwrite(rw, &_save_block, sizeof(_save_block), 1);                                       \
      return;                                                                                      \
    }                                                                                              \
    INFO("failed to write_data event data: %s\n", SAV_FILE_NAME);                                  \
  }                                                                                                \
  static void load_data(void)                                                                      \
  {                                                                                                \
    SDL_RWops* rw;                                                                                 \
    if ((rw = SDL_RWFromFile(SAV_FILE_NAME, "r")) != NULL)                                         \
    {                                                                                              \
      SDL_RWread(rw, &_save_block, sizeof(_save_block), 1);                                        \
    }                                                                                              \
    else                                                                                           \
    {                                                                                              \
      init_default_data();                                                                         \
    }                                                                                              \
  }

void game_event_init(void);
void game_event_fini(void);
void notify_game_event_finished(GameEventCode event_code);

#endif // GAME_EVENT_H
