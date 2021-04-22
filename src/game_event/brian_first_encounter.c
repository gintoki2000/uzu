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

SAVE_AND_LOAD_FN("event.brian_initial_encounter")

static void ProcessingState_on_level_loaded(void* arg, const MSG_LevelLoaded* msg);
static void ProcessingState_on_conversation_finihsed(void*                           arg,
                                                     const MSG_ConversationFinished* msg);
static void SetupProcessingState();

// begin logic section
static void ProcessingState_on_conversation_finihsed(SDL_UNUSED void*                arg,
                                                     const MSG_ConversationFinished* msg)
{
  if (msg->id == CON_BRIAN_FIRST_ENCOUNTER)
  {
    _save.state = FINISHED;
    if (!SDL_strcmp(msg->response, "accept his request"))
      ems_broadcast(MSG_EVENT_FINISHED,
                    &(MSG_EventFinished){ EVT_BRIAN_FIRST_ENCOUNTER, "accept" });
    else
      ems_broadcast(MSG_EVENT_FINISHED,
                    &(MSG_EventFinished){ EVT_BRIAN_FIRST_ENCOUNTER, "denine" });
    ems_disconnect(MSG_CONVERSATION_FINISHED, ProcessingState_on_conversation_finihsed);
    ems_disconnect(MSG_LEVEL_LOADED, ProcessingState_on_level_loaded);
  }
}
static void ProcessingState_on_level_loaded(SDL_UNUSED void* arg, const MSG_LevelLoaded* msg)
{
  if (!SDL_strcmp(msg->level_name, "demon_ruin"))
  {
    make_npc_brian(g_ecs, _brian_position, CON_BRIAN_FIRST_ENCOUNTER);
    ems_connect(MSG_CONVERSATION_FINISHED, CALLBACK_2(ProcessingState_on_conversation_finihsed));
  }
}

static void SetupProcessingState()
{
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(ProcessingState_on_level_loaded));
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
    SetupProcessingState();
}

void brian_first_encounter_fini()
{
  write_data();
}
