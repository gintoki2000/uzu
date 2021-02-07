#include "global.h"
#include "ecs/ecs.h"
#include "entity_factory.h"
#include "inventory.h"
#include "resources.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "game_event/game_event.h"
#include "ui_msgbox.h"

#define FILE_DATA_NAME "begining"

extern Ecs* g_ecs;

#define PHASE_0 0
#define PHASE_1A 1
#define PHASE_1B 2
#define PHASE_2A 3

static struct
{
  u8   state;
  BOOL you_got_the_key;
  BOOL nova_alive;
} _save_block;

static Vec2 k_nova_positions[] = {
  [PHASE_0]  = { 704.f, 54.f },
  [PHASE_1A] = { 319.f, 61.f },
};

static RECT k_zone = { 340.f, 60.f, 258.f, 195.f };

EVENT_SAVE_AND_LOAD_FN()

//<-----------------------------------event callbacks-------------------------------->//
static void on_level_loaded(pointer_t arg, const MSG_LevelLoaded* event);
static void on_game_scene_unload(pointer_t arg, const void* event);
static void phase_0_on_conversation_finished(pointer_t arg, const MSG_ConversationFinished* event);
static void on_entity_died(pointer_t arg, const MSG_EntityDied* event);
static void on_level_unload(pointer_t arg, const MSG_LevelUnloaded* event);
static void phase_1a_on_entity_died(pointer_t arg, const MSG_EntityDied* event);
//====================================================================================//

static void init_default()
{
  _save_block.state           = 0;
  _save_block.nova_alive      = TRUE;
  _save_block.you_got_the_key = FALSE;
}

static void on_level_loaded(SDL_UNUSED pointer_t arg, const MSG_LevelLoaded* event)
{
  if (!_save_block.nova_alive)
    return;

  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
  ems_connect(MSG_LEVEL_UNLOADED, NULL, on_level_unload);

  switch (_save_block.state)
  {
  case 0:
    if (strcmp(event->level_name, "1") == 0)
    {
      make_npc_nova(g_ecs, k_nova_positions[PHASE_0], CONVERSATION_NOVA_00);
      ems_connect(MSG_CONVERSATION_FINISHED, NULL, phase_0_on_conversation_finished);
    }
    break;
  case PHASE_1A:
    if (event->level_name && strcmp(event->level_name, "0") == 0)
    {
      make_npc_nova(g_ecs, k_nova_positions[PHASE_1A], CONVERSATION_NOVA_01);
    }
  }
}

static void on_level_unload(SDL_UNUSED pointer_t arg, SDL_UNUSED const MSG_LevelUnloaded* event)
{
  ems_disconnect(MSG_ENTITY_DIED, (pointer_t)on_entity_died);
}

static void on_game_scene_unload(SDL_UNUSED pointer_t arg, SDL_UNUSED const void* event)
{
  write_save_data();
}

static void phase_0_on_conversation_finished(SDL_UNUSED pointer_t            arg,
                                             const MSG_ConversationFinished* event)
{
  Item* item                      = get_item(ITEM_TYPE_RED_FLASK);
  BOOL  you_say_yes               = event->response != NULL && strcmp(event->response, "yes") == 0;
  BOOL  npc_name_is_nova          = event->npc_name != NULL && strcmp(event->npc_name, "nova") == 0;
  BOOL  you_have_enough_red_flask = item != NULL && item->num_items >= 2;
  if (npc_name_is_nova && you_say_yes)
  {
    if (you_have_enough_red_flask)
    {
      _save_block.state = PHASE_1A;
      item->num_items -= 2;
      ui_msgbox_display("give x2 red flask to Nova Knight");
      ecs_set(g_ecs, event->npc, DIALOGUE, &(Dialogue){ CONVERSATION_NOVA_01 });
      ems_connect(MSG_ENTITY_DIED, NULL, phase_1a_on_entity_died);
    }
    else
    {
      ui_msgbox_display("you do not have enough required items");
    }
  }
  else
  {
    _save_block.state = PHASE_1B;
    ecs_set(g_ecs, event->npc, DIALOGUE, &(Dialogue){ CONVERSATION_NOVA_02 });
  }
  ems_disconnect(MSG_CONVERSATION_FINISHED, (pointer_t)phase_0_on_conversation_finished);
}

static void on_entity_died(SDL_UNUSED pointer_t arg, const MSG_EntityDied* event)
{
  Name* name = ecs_get(g_ecs, event->entity, NAME);
  if (name != NULL && strcmp(name->value, "nova") == 0)
  {
    _save_block.nova_alive = FALSE;
  }
}

static BOOL __callback_collision_query(BOOL* are_there_enemy_in_zone, ecs_entity_t e)
{
  if (ecs_has(g_ecs, e, ENEMY_TAG))
  {
    *are_there_enemy_in_zone = TRUE;
    return FALSE;
  }
  return TRUE;
}

static void phase_1a_on_entity_died(SDL_UNUSED pointer_t arg, const MSG_EntityDied* event)
{
  Transform* transform = ecs_get(g_ecs, event->entity, TRANSFORM);
  BOOL       is_enemy  = ecs_has(g_ecs, event->entity, ENEMY_TAG);
  POINT      point;
  BOOL       are_there_enemy_in_zone;
  if (transform != NULL && is_enemy)
  {
    point.x = transform->position.x;
    point.y = transform->position.y;

    if (SDL_PointInRect(&point, &k_zone))
    {
      collision_box_query(&k_zone,
                          BIT(CATEGORY_ENEMY),
                          CALLBACK_1(&are_there_enemy_in_zone, __callback_collision_query));
      // TODO: player đã giết toàn bộ quái trong "zone" npc có thể quay về khu vực trung tâm an
      // toàn
      ems_disconnect(MSG_ENTITY_DIED, (pointer_t)phase_1a_on_entity_died);
    }
  }
}
void GE_begining_init(void)
{
  if (!load_save_data())
    init_default();

  ems_connect(MSG_LEVEL_LOADED, NULL, on_level_loaded);
  ems_connect(MSG_GAME_SCENE_UNLOAD, NULL, on_game_scene_unload);
}
