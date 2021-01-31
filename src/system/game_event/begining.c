
#include "ecs/ecs.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "entity_factory.h"

#define FILE_DATA_NAME "begining"

extern Ecs* g_ecs;
static struct
{
  u8 state;
} _save_block;

static Vec2 phase_0_nova_position     = { 704.f, 54.f };
static u16  phase_0_nova_converastion = CONVERSATION_DEMO1;

//<-----------------------------------event callbacks-------------------------------->//
static void on_level_loaded(pointer_t arg, const MSG_LevelLoaded* event);
static void on_game_scene_unload(pointer_t arg, const void* event);
static void phase_0_on_conversation_finished(pointer_t arg, const MSG_ConversationFinished* event);
//====================================================================================//

static void init_default()
{
  _save_block.state = 0;
}

static BOOL load_save_data()
{
  FILE* file;

  if ((file = fopen(FILE_DATA_NAME, "r")) != NULL)
  {
    fread(&_save_block, sizeof(_save_block), 1, file);
    fclose(file);
    return TRUE;
  }
  return FALSE;
}

static void write_save_data()
{
  FILE* file;

  if ((file = fopen(FILE_DATA_NAME, "w")) != NULL)
  {
    fwrite(&_save_block, sizeof(_save_block), 1, file);
    fclose(file);
  }
}

static void on_level_loaded(pointer_t arg, const MSG_LevelLoaded* event)
{
  (void)arg;

  switch (_save_block.state)
  {
  case 0:
    if (strcmp(event->level_name, "1") == 0)
    {
      // TODO: create npc
      make_npc_nova(g_ecs, phase_0_nova_position, phase_0_nova_converastion);          
      ems_connect(MSG_CONVERSATION_FINISHED, NULL, phase_0_on_conversation_finished);
    }
    break;
  }
}

static void on_game_scene_unload(pointer_t arg, const void* event)
{
  (void)arg;
  (void)event;
  write_save_data();
}

static void phase_0_on_conversation_finished(pointer_t arg, const MSG_ConversationFinished* event)
{
  
}

void GE_begining_init(void)
{
  if (!load_save_data())
    init_default();

  ems_connect(MSG_LEVEL_LOADED, NULL, on_level_loaded);
  ems_connect(MSG_GAME_SCENE_UNLOAD, NULL, on_game_scene_unload);
}
