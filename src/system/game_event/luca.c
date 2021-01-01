#include "luca.h"
#include "../mediator.h"
#include "components.h"

extern Ecs* g_ecs;

//<-------------------------event callback--------------------------------------------->//

static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event);

//======================================================================================//

void game_event_luca_init()
{
  mediator_connect(SYS_SIG_FINISH_CONVERSATION, NULL, SLOT(on_finish_conversation));
}

static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event)
{
  (void)arg;
  if (strcmp(event->npc_name, "luca") == 0 &&
      strcmp(event->conversation_name, "luca_greeting") == 0)
  {
    Interactable* interacable;

    interacable = ecs_get(g_ecs, event->npc, INTERACTABLE);

    interacable->cnt = 2;
    interacable_set_cmd(interacable, 1, "LEARN");

    mediator_disconnect(SYS_SIG_FINISH_CONVERSATION, (pointer_t)on_finish_conversation);
  }
}
