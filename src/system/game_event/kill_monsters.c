#include "game_event.h"
#include "game_scene.h"
#include "resources.h"
#include <components.h>
#include <inventory.h>
#include <system/mediator.h>
#include <utils.h>

static u8 _state; // INACTIVE, INPROGRESS, FINISHED
static u8 _killed_monster_cnt;

extern Ecs* g_ecs;

//<--------------------------event callback----------------------------->//
static void on_entity_died(pointer_t arg, const SysEvt_EntityDied* event);
static void on_game_event_finished(pointer_t arg, const SysEvt_FinishEvent* event);
static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event);
static void on_game_scene_unload(pointer_t arg, const pointer_t event);
//<=====================================================================>//

void GE_kill_monsters_init(void)
{

  FILE* save_file;

  if ((save_file = fopen("kill_monster", "r")) != NULL)
  {
    u8 data[2];
    fread(data, 1, 2, save_file);

    _state              = data[0];
    _killed_monster_cnt = data[1];
    fclose(save_file);
  }
  else
  {
    //đặt về giá trị mặc định
    _state              = 0;
    _killed_monster_cnt = 0;
  }

  mediator_connect(SYS_SIG_FINISH_EVENT, NULL, SLOT(on_game_event_finished));
  game_scene_connect_sig(GAME_SCENE_UNLOAD, on_game_scene_unload, NULL);
}

static void on_entity_died(pointer_t arg, const SysEvt_EntityDied* event)
{
  (void)arg;
  if (ecs_has(g_ecs, event->entity, ENEMY_TAG))
  {
    _killed_monster_cnt++;

    if (_killed_monster_cnt == 3)
    {
      ecs_entity_t luca;
      Dialogue*    dialogue;

      luca = find_entity(g_ecs, "luca");
      ASSERT(luca != ECS_NULL_ENT);

      dialogue                  = ecs_get(g_ecs, luca, DIALOGUE);
      dialogue->conversation_id = CONVERSATION_DEMO3;

      mediator_disconnect(SYS_SIG_ENTITY_DIED, (pointer_t)on_entity_died);
      mediator_connect(SYS_SIG_FINISH_CONVERSATION, NULL, SLOT(on_finish_conversation));
    }
  }
}

static void on_game_event_finished(pointer_t arg, const SysEvt_FinishEvent* event)
{
  (void)arg;
  if (event->event_code == GAME_EVENT_GET_WEAPON)
  {
    mediator_disconnect(SYS_SIG_FINISH_EVENT, (pointer_t)on_game_event_finished);
    mediator_connect(SYS_SIG_ENTITY_DIED, NULL, SLOT(on_entity_died));
  }
}

static void on_finish_conversation(pointer_t arg, const SysEvt_FinishConversation* event)
{
  (void)arg;

  if (strcmp(event->conversation_name, "demo3") == 0)
  {
    add_to_inv(ITEM_TYPE_BIG_RED_FLASK);

    mediator_disconnect(SYS_SIG_FINISH_CONVERSATION, (pointer_t)on_finish_conversation);
    mediator_broadcast(SYS_SIG_FINISH_EVENT, &(SysEvt_FinishEvent){ GAME_EVENT_KILL_MONSTER });
  }
}

static void on_game_scene_unload(pointer_t arg, const pointer_t event)
{
  (void)arg;
  (void)event;
  FILE* save_file;
  if ((save_file = fopen("kill_monster", "w")) != NULL)
  {
    fwrite((u8[]){ _state, _killed_monster_cnt }, 1, 2, save_file);
    fclose(save_file);
  }
}
