#include "entity_factory.h"
#include "game_event.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"

enum
{
  PROCESSING,
  FINISHED
};

static struct
{
  u32 state;
} _save;

static Vec2 _brian_position = { 47 * 16, 7 * 16 };
extern Ecs* g_ecs;

GAME_EVENT("event.brian_initial_encounter")

static void processing();
static void PROCESSING_on_level_loaded(void* arg, const LevelLoadedMsg* msg);
static void PROCESSING_on_conversation_finished(void* arg, const MSG_ConversationFinished* msg);

// begin logic section
static void PROCESSING_on_conversation_finished(SDL_UNUSED void*                arg,
                                                const MSG_ConversationFinished* msg)
{
  if (msg->id == CON_BRIAN_FIRST_ENCOUNTER)
  {
    _save.state = FINISHED;
    if (!SDL_strcmp(msg->response, "accept his request"))
      ems_broadcast(MSG_EVENT_FINISHED, &(EventFinishedMsg){ EVT_BRIAN_FIRST_ENCOUNTER, "accept" });
    else
      ems_broadcast(MSG_EVENT_FINISHED, &(EventFinishedMsg){ EVT_BRIAN_FIRST_ENCOUNTER, "denine" });
    ems_disconnect(MSG_CONVERSATION_FINISHED, PROCESSING_on_conversation_finished);
    ems_disconnect(MSG_LEVEL_LOADED, PROCESSING_on_level_loaded);
  }
}
static void PROCESSING_on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* msg)
{
  if (!SDL_strcmp(msg->level_name, "demon_ruin"))
  {
    make_npc_brian(g_ecs, _brian_position, CON_BRIAN_FIRST_ENCOUNTER);
    ems_connect(MSG_CONVERSATION_FINISHED, CALLBACK_2(PROCESSING_on_conversation_finished));
  }
}

static void processing()
{
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(PROCESSING_on_level_loaded));
}
// end logic section

static void init_default_data()
{
  _save.state = PROCESSING;
}

void brian_first_encounter_init()
{
  load_data();
  if (_save.state == PROCESSING)
    processing();
}

void brian_first_encounter_fini()
{
  write_data();
}
