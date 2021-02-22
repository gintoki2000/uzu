#ifndef GAME_EVENT_H
#define GAME_EVENT_H

typedef enum
{
  GAME_EVENT_GET_WEAPON,
  GAME_EVENT_KILL_MONSTER,
} GameEventCode;

#define EVENT_SAVE_AND_LOAD_FN(__file_name)                                                        \
  static BOOL load_save_data()                                                                     \
  {                                                                                                \
    FILE* file;                                                                                    \
    if ((file = fopen(__file_name, "r")) != NULL)                                                  \
    {                                                                                              \
      fread(&_save_block, sizeof(_save_block), 1, file);                                           \
      fclose(file);                                                                                \
      return TRUE;                                                                                 \
    }                                                                                              \
    return FALSE;                                                                                  \
  }                                                                                                \
  static void write_save_data()                                                                    \
  {                                                                                                \
    FILE* file;                                                                                    \
    if ((file = fopen(__file_name, "w")) != NULL)                                                  \
    {                                                                                              \
      fwrite(&_save_block, sizeof(_save_block), 1, file);                                          \
      fclose(file);                                                                                \
    }                                                                                              \
  }

void game_event_init(void);
void game_event_fini(void);
void notify_game_event_finished(GameEventCode event_code);

#endif // GAME_EVENT_H
