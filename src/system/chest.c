#include "components.h"
#include "constances.h"
#include "inventory.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include "ui_msgbox.h"
#include "ui_msgbox_w_icon.h"
#include <string.h>

extern ecs_Registry* gRegistry;

static void on_level_loaded(void*, const LevelLoadedMsg*);
static void on_level_unload(void*, const LevelUnloadedMsg*);

static void open_chest(ecs_entity_t entity)
{
  ChestAttributes* attrs;
  Visual*          visual;

  attrs = ecs_get(gRegistry, entity, CHEST);

  if (attrs->numSlots == 0)
  {
    visual              = ecs_get(gRegistry, entity, VISUAL);
    visual->sprite.rect = gRectChestOpen;
    attrs->state        = CHEST_STATE_OPEN;
    ui_msgbox_display("chest empty!");
  }
  else
  {
    UI_MsgBoxWIcon_Entry entries[attrs->numSlots];
    for (int i = 0; i < attrs->numSlots; ++i)
    {
      inv_add_item(attrs->items[i].type, attrs->items[i].quality);
      SDL_strlcpy(entries[i].text,
                  gItemTypes[attrs->items[i].type].name,
                  UI_MSGBOX_W_ICON_MAX_TEXT_LEN);
      entries[i].icon = gItemTypes[attrs->items[i].type].icon;
    }
    ui_msgbox_w_icon_show(entries, attrs->numSlots);
    attrs->numSlots     = 0;
    visual              = ecs_get(gRegistry, entity, VISUAL);
    visual->sprite.rect = gRectChestOpen;
    attrs->state        = CHEST_STATE_OPEN;
  }
}

static void on_command_selected(pointer_t arg, const CommandSelectedMsg* event)
{
  (void)arg;
  if (!strcmp(event->cmd, gCmdOpen) && ecs_has(gRegistry, event->entity, CHEST))
    open_chest(event->entity);
}

static void on_level_unload(SDL_UNUSED void* arg, const LevelUnloadedMsg* event)
{
  char       sav[256];
  SDL_RWops* file;

  ChestAttributes* chest;
  ecs_size_t       numChests;

  SDL_strlcpy(sav, "chest.", 255);
  SDL_strlcat(sav, event->level_name, 255);

  if ((file = SDL_RWFromFile(sav, "w")) != NULL)
  {
    ecs_raw(gRegistry, CHEST, NULL, (void**)&chest, &numChests);
    SDL_RWwrite(file, &numChests, sizeof(numChests), 1);
    SDL_RWwrite(file, chest, sizeof(ChestAttributes), numChests);
    SDL_RWclose(file);
  }
}

static void on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* event)
{
  char       sav[256];
  SDL_RWops* file;

  ChestAttributes* savData;
  ChestAttributes* chest;
  ecs_size_t       numSavObjs;
  ecs_size_t       numChests;

  SDL_strlcpy(sav, event->level_name, 255);
  SDL_strlcat(sav, ".item", 255);

  if ((file = SDL_RWFromFile(sav, "r")) != NULL)
  {
    SDL_RWread(file, &numSavObjs, sizeof(ecs_size_t), 1);
    savData = SDL_malloc(numSavObjs * sizeof(ChestAttributes));
    SDL_RWread(file, savData, sizeof(ChestAttributes), numSavObjs);

    ecs_raw(gRegistry, CHEST, NULL, (void**)&chest, &numChests);
    for (int ichest = 0; ichest < numChests; ++ichest)
    {
      for (int isaveobj = 0; isaveobj < numSavObjs; ++isaveobj)
      {
        if (chest[ichest].id == savData[isaveobj].id)
          SDL_memcpy(&chest[ichest], &savData[isaveobj], sizeof(ChestAttributes));
      }
    }
    SDL_free(savData);
    SDL_RWclose(file);
  }
}

void chest_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(on_level_loaded));
  ems_connect(MSG_LEVEL_UNLOADED, CALLBACK_2(on_level_unload));
}
