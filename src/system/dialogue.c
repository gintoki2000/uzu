#include "system/game_logic.h"

#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "entity_utils.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "ui_dialogue.h"
#include "ui_list.h"

static Conversation* _conversation;
static Queue         _queue;
static u16           _conversation_id;
static ecs_entity_t  _npc;
extern Ecs*          g_ecs;

static void process_input(void* arg);
static BOOL next_sentence(void);
static void on_text_selected(void* arg, const char* text);

void display_dialogue(u16 conversation_id, ecs_entity_t entity)
{
  _conversation    = get_conversation(conversation_id);
  _conversation_id = conversation_id;
  _npc             = entity;
  queue_clear(&_queue);
  for (int i = 0; i < _conversation->sentences->cnt; ++i)
    queue_offer(&_queue, _conversation->sentences->storage[i]);
  ui_dialogue_set_visible(TRUE);
  next_sentence();
  input_push_state(INPUT_STATE_INST_1(process_input));
}

static void display_responses(void)
{
  ui_dialogue_set_visible(FALSE);
  ui_list_display((const char**)_conversation->responses->storage, _conversation->responses->cnt);
  ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_text_selected));
}

static void end_conversation(const char* res)
{
  ui_dialogue_set_visible(FALSE);
  input_pop_state();
  ems_broadcast(
      MSG_CONVERSATION_FINISHED,
      &(MSG_ConversationFinished){ .npc = _npc, .response = res, .id = _conversation_id });
}

static void on_text_selected(SDL_UNUSED void* arg, const char* text)
{
  end_conversation(text);
}

static void process_input(SDL_UNUSED void* arg)
{
  if (button_just_pressed(BUTTON_INTERACT))
  {
    if (!next_sentence())
      _conversation->responses->cnt > 0 ? display_responses() : end_conversation(NULL);
  }
}

static BOOL next_sentence(void)
{
  if (!queue_empty(&_queue))
  {
    ui_dialogue_set_text(queue_poll(&_queue));
    return TRUE;
  }
  return FALSE;
}

static void on_command_selected(SDL_UNUSED void* arg, const MSG_CommandSelected* msg)
{
  if (!SDL_strcmp(msg->cmd, TEXT_COMMAND_TALK) && ecs_has(g_ecs, msg->entity, DIALOGUE))
  {
    display_dialogue(ett_get_conversation(g_ecs, msg->entity), msg->entity);
  }
}

void dialogue_system_init()
{
  queue_init(&_queue, 32);
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
}

void dialogue_system_fini()
{
  queue_fini(&_queue);
}

void dialogue_system()
{
}
