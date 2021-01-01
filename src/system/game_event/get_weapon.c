#include "../mediator.h"
#include "components.h"
#include "game_event.h"
#include <entity_factory.h>
#include <utils.h>

extern Ecs* g_ecs;

//<--------------------------------------------------------------------------------->//
static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event);
//<===========================================================================>//

void GE_get_weapon_init(void)
{
  mediator_connect(SYS_SIG_FINISH_CONVERSATION, NULL, SLOT(on_finish_conversation));
}

static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event)
{
  (void)arg;
  if (strcmp(event->conversation_name, "conversation 1") == 0)
  {

    // thay đổi lời thoại của NPC
    Dialogue* dialogue;

    dialogue = ecs_get(g_ecs, event->npc, DIALOGUE);
    dialogue_clear(dialogue);
    dialogue_set_name(dialogue, "conversation 2");
    dialogue_add_sentence(dialogue, "Go and kill those monsters...");

    //đưa vũ khí cho player

    ecs_entity_t weapon = make_golden_sword(g_ecs, BIT(CATEGORY_ENEMY));
    equip(g_ecs, get_player(g_ecs), weapon);

    mediator_disconnect(SYS_SIG_FINISH_CONVERSATION, (pointer_t)on_finish_conversation);
    mediator_broadcast(SYS_SIG_FINISH_EVENT, &(SysEvt_FinishEvent){ GAME_EVENT_GET_WEAPON });
  }
}
