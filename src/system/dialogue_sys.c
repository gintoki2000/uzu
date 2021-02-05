#include "system/game_logic.h"

#include "entity_utils.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "ui_dialogue.h"
#include "ui_helper.h"

//<-------------------------------------event callbacks---------------------------------->//

static void on_command_selected(pointer_t arg, const MSG_CommandSelected* event);
static void on_next_sentence(pointer_t arg, const char*);
static void on_dialogue_finished(pointer_t arg, const UI_DialogueEvent_DialogueFinished* event);

//<======================================================================================>//

extern Ecs* g_ecs;

static ecs_entity_t _npc;

static void begin_conversation(ecs_entity_t entity)
{
  _npc = entity;
  ui_dialogue_hook(UI_DIALOGUE_EVENT_NEXT_SENTENCE, CALLBACK_2(on_next_sentence));
  ui_dialogue_hook(UI_DIALOGUE_EVENT_DIALOGUE_FINISHED, CALLBACK_2(on_dialogue_finished));
  ui_dialogue_show(get_entity_conversation(g_ecs, entity));
}

static void on_command_selected(pointer_t arg, const MSG_CommandSelected* event)
{
  (void)arg;
  if (strcmp(event->cmd, TEXT_COMMAND_TALK) == 0)
  {
    begin_conversation(event->entity);
  }
}

static void on_next_sentence(pointer_t arg, const char* sentence)
{
  (void)arg;
  (void)sentence;
}

static void on_dialogue_finished(pointer_t arg, const UI_DialogueEvent_DialogueFinished* event)
{
  (void)arg;
  ems_broadcast(MSG_CONVERSATION_FINISHED,
                &(MSG_ConversationFinished){ .npc               = _npc,
                                             .npc_name          = get_entity_name(g_ecs, _npc),
                                             .conversation_name = event->name,
                                             .response          = event->response });
}

void dialogue_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, NULL, on_command_selected);
}

void dialogue_system_fini()
{
}

void dialogue_system_update()
{
}
