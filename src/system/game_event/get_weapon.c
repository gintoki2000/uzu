#include "components.h"
#include "game_event.h"
#include <entity_factory.h>
#include <utils.h>
#include "../event_messaging_sys.h"

extern Ecs* g_ecs;

enum {
	GET_WEAPON_EVT_STATE_INITIAL,
	GET_WEAPON_EVT_STATE_FINISHED,
};

static u8 _state;

//<--------------------------------------------------------------------------------->//
static void on_finish_conversation(pointer_t arg, const MSG_ConversationFinished* event);
//<===========================================================================>//

void GE_get_weapon_init(void)
{
  ems_connect(MSG_CONVERSATION_FINISHED, NULL, SLOT(on_finish_conversation));
}

static void on_finish_conversation(pointer_t arg, const MSG_ConversationFinished* event)
{
  (void)arg;
  if (strcmp(event->conversation_name, "demo1") == 0 && strcmp(event->response, "yes") == 0)
  {

    // thay đổi lời thoại của NPC
    Dialogue* dialogue;

    dialogue                  = ecs_get(g_ecs, event->npc, DIALOGUE);
    dialogue->conversation_id = CONVERSATION_DEMO2;

    //đưa vũ khí cho player

    ecs_entity_t weapon = make_golden_sword(g_ecs, BIT(CATEGORY_ENEMY));
    equip(g_ecs, get_player(g_ecs), weapon);

    ems_disconnect(MSG_CONVERSATION_FINISHED, (pointer_t)on_finish_conversation);
    notify_game_event_is_finished(GAME_EVENT_GET_WEAPON);
  }
}
