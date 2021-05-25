#include "entity_factory.h"
#include "entity_utils.h"
#include "game_event.h"
#include "inventory.h"
#include "session.h"
#include "system/event_messaging_sys.h"
#include "toolbox/toolbox.h"
#include "ui_msgbox.h"

enum
{
  INACTIVE,
  PROCESSING,
  PICKUP_BRIAN,
  FINHISHED
};

#define NUM_ENEMIES 4

static struct
{
  int  state;
  BOOL alive[NUM_ENEMIES];
  int  count;
} _save;

static const Vec2 _enemy_position_tbl[] = {
  { 71.f, 24.f },
  { 86.f, 100.f },
  { 45.f, 129.f },
  { 64.f, 266.f },
};
static ecs_entity_t _enemies[NUM_ENEMIES];
static Vec2         _brian_position = { 47 * 16, 7 * 16 };
extern Ecs*         gEcs;

GAME_EVENT("event.brian_rescue")
static void processing();
static void inactive();
static void pickup();
static void INACTIVE_on_event_finished(void* arg, const EventFinishedMsg* msg);
static void PROCESSING_on_level_loaded(void* arg, const LevelLoadedMsg* msg);
static void PROCESSING_on_entity_died(void* arg, const EntityDiedMsg* msg);
static void PICKUP_on_level_loaded(void* arg, const LevelLoadedMsg* msg);
static void PICKUP_on_converastion_finished(void* arg, const MSG_ConversationFinished* msg);
static void PICKUP_on_msgbox_close(void* arg, const void* msg);

static Action* create_action_sequence();

static void init_default_data()
{
  _save.state = INACTIVE;
  _save.count = NUM_ENEMIES;
  SDL_memset4(_save.alive, TRUE, NUM_ENEMIES);
}

static void INACTIVE_on_event_finished(SDL_UNUSED void* arg, const EventFinishedMsg* msg)
{
  if (msg->code == EVT_BRIAN_FIRST_ENCOUNTER)
  {
    if (!SDL_strcmp(msg->status, "accept"))
    {
      _save.state        = PROCESSING;
      ecs_entity_t brian = scn_find(gEcs, "brian");
      if (brian != ECS_NULL_ENT)
        ecs_set(gEcs, brian, DIALOGUE, &(Dialogue){ CON_BRIAN_RESCUE_1 });
      processing();
    }
    else
    {
      _save.state = FINHISHED;
    }
    ems_disconnect(MSG_EVENT_FINISHED, INACTIVE_on_event_finished);
  }
}

static void PROCESSING_on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* msg)
{
  if (!SDL_strcmp(msg->level_name, "corridor"))
  {
    for (int i = 0; i < NUM_ENEMIES; ++i)
    {
      if (_save.alive[i])
        _enemies[i] = make_chort(gEcs, _enemy_position_tbl[i]);
    }
    ems_connect(MSG_ENTITY_DIED, CALLBACK_2(PROCESSING_on_entity_died));
  }
  else if (!SDL_strcmp(msg->level_name, "demon_ruin"))
  {
    make_npc_brian(gEcs, _brian_position, CON_BRIAN_RESCUE_1);
  }
}

static void PICKUP_on_level_loaded(SDL_UNUSED void* arg, const LevelLoadedMsg* msg)
{
  if (!SDL_strcmp(msg->level_name, "demon_ruin"))
  {
    make_npc_brian(gEcs, _brian_position, CON_BRIAN_RESCUE_2);
    ems_connect(MSG_CONVERSATION_FINISHED, CALLBACK_2(PICKUP_on_converastion_finished));
  }
}

static void PROCESSING_on_entity_died(SDL_UNUSED void* arg, const EntityDiedMsg* msg)
{
  if (!SDL_strcmp(gSession.level, "corridor"))
  {
    for (int i = 0; i < NUM_ENEMIES; ++i)
    {
      if (_save.alive[i] && msg->entity == _enemies[i])
      {
        _save.alive[i] = FALSE;
        --_save.count;
        if (_save.count == 0)
        {
          _save.state = PICKUP_BRIAN;
          pickup();
          ems_disconnect(MSG_ENTITY_DIED, PROCESSING_on_entity_died);
          ems_disconnect(MSG_LEVEL_LOADED, PROCESSING_on_level_loaded);
        }
      }
    }
  }
}

static void PICKUP_on_msgbox_close(SDL_UNUSED void* arg, SDL_UNUSED const void* msg)
{
  ecs_entity_t brian = scn_find(gEcs, "brian");
  ASSERT(brian != ECS_NULL_ENT);
  ecs_set(gEcs, brian, SCRIPT, &(Script){ create_action_sequence(), { 0 } });
}

static void PICKUP_on_converastion_finished(SDL_UNUSED void*                arg,
                                            const MSG_ConversationFinished* msg)
{
  if (msg->id == CON_BRIAN_RESCUE_2)
  {
    if (!SDL_strcmp(msg->response, "Give him some recovery medicine"))
    {
      Item* item = inv_get_item(ITEM_TYPE_RED_FLASK);
      if (item->quality >= 3)
      {
        item->quality -= 3;
        ui_msgbox_display("give to brian x3 red flasks");
        ui_msgbox_close_hook(CALLBACK_2(PICKUP_on_msgbox_close));
        _save.state = FINHISHED;
        ems_broadcast(MSG_EVENT_FINISHED, &(EventFinishedMsg){ EVT_BRIAN_RECUSE, "accept" });
      }
    }
    else
    {
      ems_broadcast(MSG_EVENT_FINISHED, &(EventFinishedMsg){ EVT_BRIAN_RECUSE, "denie" });
    }
    _save.state = FINHISHED;
    ems_disconnect(MSG_LEVEL_LOADED, PROCESSING_on_level_loaded);
    ems_disconnect(MSG_CONVERSATION_FINISHED, PICKUP_on_converastion_finished);
  }
}

static Action* create_action_sequence()
{
  CompositeAction* group1 = parallel_action_new();
  composite_action_add(group1, talk_action_new(CON_BRIAN_RESCUE_3));
  composite_action_add(group1, express_emotion_action_new(EMOJI_HEART, 180));

  CompositeAction* seq = sequence_action_new();
  composite_action_add(seq, ACTION(group1));
  composite_action_add(seq, walk_action_new((Vec2){ 774.f, 71.f }));
  composite_action_add(seq, walk_action_new((Vec2){ 906.f, 74.f }));
  composite_action_add(seq, walk_action_new((Vec2){ 906.f, 74.f }));
  composite_action_add(seq, walk_action_new((Vec2){ 905.f, 19.f }));
  composite_action_add(seq, vanish_action_new());

  return (Action*)seq;
}

static void processing()
{
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(PROCESSING_on_level_loaded));
}

static void inactive()
{
  ems_connect(MSG_EVENT_FINISHED, CALLBACK_2(INACTIVE_on_event_finished));
}

static void pickup()
{
  ems_connect(MSG_LEVEL_LOADED, CALLBACK_2(PICKUP_on_level_loaded));
}

void brian_rescue_init()
{
  load_data();
  switch (_save.state)
  {
  case INACTIVE:
    inactive();
    break;
  case PROCESSING:
    processing();
    break;
  case PICKUP_BRIAN:
    pickup();
    break;
  default:
    break;
  }
}

void brian_rescue_fini()
{
  write_data();
}
