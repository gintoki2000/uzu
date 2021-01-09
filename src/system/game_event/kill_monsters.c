#include "../event_messaging_sys.h"
#include "game_event.h"
#include "game_scene.h"
#include <components.h>
#include <inventory.h>
#include <resources.h>
#include <ui_msgbox.h>
#include <utils.h>

static u8 _state; // INACTIVE, INPROGRESS, FINISHED
static u8 _killed_monster_cnt;

extern Ecs* g_ecs;



//<--------------------------event callback----------------------------->//
static void on_entity_died(pointer_t arg, const MSG_EntityDied* event);
static void on_game_event_finished(pointer_t arg, const MSG_EventFinished* event);
static void on_conversation_finished(pointer_t arg, const MSG_ConversationFinished* event);
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

  ems_connect(MSG_EVENT_FINISHED, NULL, on_game_event_finished);
  ems_connect(MSG_GAME_SCENE_UNLOAD, NULL, on_game_scene_unload);
}

static void on_entity_died(pointer_t arg, const MSG_EntityDied* event)
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

      ems_disconnect(MSG_ENTITY_DIED, (pointer_t)on_entity_died);
      ems_connect(MSG_CONVERSATION_FINISHED, NULL, on_conversation_finished);
    }
  }
}

static void on_game_event_finished(pointer_t arg, const MSG_EventFinished* event)
{
  (void)arg;
  if (event->event_code == GAME_EVENT_GET_WEAPON)
  {
    ems_disconnect(MSG_EVENT_FINISHED, (pointer_t)on_game_event_finished);
    ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
  }
}

static void on_conversation_finished(pointer_t arg, const MSG_ConversationFinished* event)
{
  (void)arg;

  if (strcmp(event->conversation_name, "demo3") == 0)
  {
    add_to_inv(ITEM_TYPE_BIG_RED_FLASK, 3);
    ui_msgbox_display("you got x1 BIG RED FLASK");
    ems_disconnect(MSG_CONVERSATION_FINISHED, (pointer_t)on_conversation_finished);
    notify_game_event_is_finished(GAME_EVENT_KILL_MONSTER);
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
