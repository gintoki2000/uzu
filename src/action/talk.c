#include "action.h"
#include "resources.h"
#include "system/event_messaging_sys.h"

extern void display_dialogue(u16 conversation_id, ecs_entity_t entity);

typedef struct TalkAction
{
  Action parent_instance;
  u16    conversation_id;
  BOOL   finished;
} TalkAction;

static void on_conversation_finished(TalkAction* self, const MSG_ConversationFinished* msg)
{
  if (self->conversation_id == msg->id)
    self->finished = TRUE;
}

static void start(void* _self, ecs_entity_t target)
{
  display_dialogue(CONVERSATION_NOVA_2A_1, target);
  ems_connect(MSG_CONVERSATION_FINISHED, CALLBACK_1(_self, on_conversation_finished));
}

static void end(void* _self, SDL_UNUSED ecs_entity_t target)
{
  ems_disconnect_ex(MSG_CONVERSATION_FINISHED, _self, on_conversation_finished);
}

static BOOL update(void* _self, SDL_UNUSED ecs_entity_t target)
{
  return ((TalkAction*)_self)->finished;
}

static ActionVtbl _talk_action_vtbl = {
  .start   = start,
  .end     = end,
  .update  = update,
  .cleanup = action_default_cleanup_func,
};

Action* talk_action_new(u16 conversation_id)
{
  // alloc
  TalkAction* action   = SDL_malloc(sizeof(TalkAction));
  ACTION(action)->vtbl = &_talk_action_vtbl;

  // init
  action->conversation_id = conversation_id;
  action->finished        = FALSE;

  return ACTION(action);
}
