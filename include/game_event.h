#ifndef GAME_EVENT_H
#define GAME_EVENT_H

typedef enum
{
  EVT_BRIAN_FIRST_ENCOUNTER,
} GameEventCode;

#define SAVE_AND_LOAD_FN(file_name)                                                                \
  static void init_default_data(void);                                                             \
  static void write_data(void)                                                                     \
  {                                                                                                \
    SDL_RWops* rw;                                                                                 \
    if ((rw = SDL_RWFromFile(file_name, "w")) != NULL)                                             \
      SDL_RWwrite(rw, &_save, sizeof(_save), 1);                                                   \
    else                                                                                           \
      INFO("failed to write_data event data: %s\n", file_name);                                    \
  }                                                                                                \
  static void load_data(void)                                                                      \
  {                                                                                                \
    SDL_RWops* rw;                                                                                 \
    if ((rw = SDL_RWFromFile(file_name, "r")) != NULL)                                             \
      SDL_RWread(rw, &_save, sizeof(_save), 1);                                                    \
    else                                                                                           \
      init_default_data();                                                                         \
  }

void game_event_init(void);
void game_event_fini(void);

#endif // GAME_EVENT_H
