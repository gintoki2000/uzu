#include "components.h"
#include "constances.h"
#include "inventory.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include "ui_msgbox.h"
#include "ui_msgbox_w_icon.h"
#include <string.h>

extern Ecs* g_ecs;

static void on_level_loaded(void*, const MSG_LevelLoaded*);
static void on_level_unload(void*, const MSG_LevelUnloaded*);

static void open_chest(ecs_entity_t entity)
{
  ChestAttributes* attrs;
  Visual*          visual;

  attrs = ecs_get(g_ecs, entity, CHEST);

  if (attrs->num_slots == 0)
  {
    visual              = ecs_get(g_ecs, entity, VISUAL);
    visual->sprite.rect = RECT_CHEST_OPEN;
    attrs->state        = CHEST_STATE_OPEN;
    ui_msgbox_display("chest empty!");
  }
  else
  {
    UI_MsgBoxWIcon_Entry entries[attrs->num_slots];
    for (int i = 0; i < attrs->num_slots; ++i)
    {
      inv_add_item(attrs->items[i].type, attrs->items[i].quality);
      SDL_strlcpy(entries[i].text,
                  g_item_types[attrs->items[i].type].name,
                  UI_MSGBOX_W_ICON_MAX_TEXT_LEN);
      entries[i].icon = g_item_types[attrs->items[i].type].icon;
    }
    ui_msgbox_w_icon_show(entries, attrs->num_slots);
    attrs->num_slots    = 0;
    visual              = ecs_get(g_ecs, entity, VISUAL);
    visual->sprite.rect = RECT_CHEST_OPEN;
    attrs->state        = CHEST_STATE_OPEN;
  }
}

static void on_command_selected(pointer_t arg, const MSG_CommandSelected* event)
{
  (void)arg;
  if (!strcmp(event->cmd, TEXT_COMMAND_OPEN) && ecs_has(g_ecs, event->entity, CHEST))
    open_chest(event->entity);
}

static void on_level_unload(SDL_UNUSED void* arg, const MSG_LevelUnloaded* event)
{
  char       sav[256];
  SDL_RWops* file;

  ChestAttributes* chest;
  ecs_size_t       num_chests;

  SDL_strlcpy(sav, "chest.", 255);
  SDL_strlcat(sav, event->level_name, 255);

  if ((file = SDL_RWFromFile(sav, "w")) != NULL)
  {
    ecs_raw(g_ecs, CHEST, NULL, (void**)&chest, &num_chests);
    SDL_RWwrite(file, &num_chests, sizeof(num_chests), 1);
    SDL_RWwrite(file, chest, sizeof(ChestAttributes), num_chests);
    SDL_RWclose(file);
  }
}

static void on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* event)
{
  char       sav[256];
  SDL_RWops* file;

  ChestAttributes* sav_data;
  ChestAttributes* chest;
  ecs_size_t       num_savobjs;
  ecs_size_t       num_chests;

  SDL_strlcpy(sav, event->level_name, 255);
  SDL_strlcat(sav, ".item", 255);

  if ((file = SDL_RWFromFile(sav, "r")) != NULL)
  {
    SDL_RWread(file, &num_savobjs, sizeof(ecs_size_t), 1);
    sav_data = SDL_malloc(num_savobjs * sizeof(ChestAttributes));
    SDL_RWread(file, sav_data, sizeof(ChestAttributes), num_savobjs);

    ecs_raw(g_ecs, CHEST, NULL, (void**)&chest, &num_chests);
    for (int ichest = 0; ichest < num_chests; ++ichest)
    {
      for (int isaveobj = 0; isaveobj < num_savobjs; ++isaveobj)
      {
        if (chest[ichest].id == sav_data[isaveobj].id)
          SDL_memcpy(&chest[ichest], &sav_data[isaveobj], sizeof(ChestAttributes));
      }
    }
    SDL_free(sav_data);
    SDL_RWclose(file);
  }
}

void chest_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(on_level_loaded));
  ems_connect(MSG_LEVEL_UNLOADED, CALLBACK_2(on_level_unload));
}
