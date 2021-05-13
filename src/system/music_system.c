#include "system/event_messaging_sys.h"

#include "resources.h"

static Mix_Music* music_from_level_name(const char* level_name)
{
  static struct
  {
    const char* level_name;
    u16         mus_id;
  } lut[] = {
    { "central", BG_MUS_BOSS },
    { "corridor", BG_MUS_LV2 },
    { "demon_ruin", BG_MUS_LV1 },
    { 0 },
  };
  for (int i = 0; lut[i].level_name != NULL; ++i)
    if (!SDL_strcmp(level_name, lut[i].level_name))
      return get_bg_mus(lut[i].mus_id);
  return NULL;
}

static void on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* event)
{
  Mix_Music* mus = music_from_level_name(event->level_name);
  if (mus != NULL)
    Mix_PlayMusic(mus, -1);
}

void music_system_init(void)
{
}
