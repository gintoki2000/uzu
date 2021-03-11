#include "ecs/ecs.h"
#include "entity_factory.h"
#include "game_event/game_event.h"
#include "global.h"
#include "inventory.h"
#include "resources.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "ui_msgbox.h"
#include "ui_subtile.h"

#define SAV_FILE_NAME "event.begining"
extern Ecs* g_ecs;
enum
{
  PHASE_INITIAL,
};

static struct
{
  u8 phase;
  BOOL is_nova_alive;
} _save_block;

static Vec2 _npc_location_tbl[] = { { 710.f, 90.f } };

SAVE_AND_LOAD_FN()
//*event callback*//
static void on_scene_unload(void* arg, const void* msg);
static void on_level_loaded(void* arg, const MSG_LevelLoaded* msg);
static void on_entity_died(void* arg, const MSG_EntityDied* msg);

static void init_default_data(void)
{
  _save_block.is_nova_alive = TRUE;
  _save_block.phase = PHASE_INITIAL;
}

static void on_scene_unload(SDL_UNUSED void* arg, SDL_UNUSED const void* msg)
{
  write_data();
}

static void on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* msg)
{
  switch (_save_block.phase)
  {
  case PHASE_INITIAL:
    if (SDL_strcmp(msg->level_name, "demon_ruin") == 0)
      make_npc_nova(g_ecs, _npc_location_tbl[PHASE_INITIAL], CONVERSATION_NOVA_00);
    break;
  }
}

static void on_entity_died(void* arg, const MSG_EntityDied* msg)
{
  
}

void GE_begining_init(void)
{
  load_data();
  ems_connect(MSG_GAME_SCENE_UNLOAD, NULL, on_scene_unload);
  ems_connect(MSG_LEVEL_LOADED, NULL, on_level_loaded);
}
