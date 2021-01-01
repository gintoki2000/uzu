#include "dialogue_sys.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "mediator.h"
#include "resources.h"
#include "ui_helper.h"
extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

static Queue        _queue;
static const char*  _current_sentence;
static FONT*        _font;
static const char*  _npc_name;
static const char*  _conversation_name;
static int          _box_x;
static int          _box_y;
static ecs_entity_t _npc = ECS_NULL_ENT; // current npc who player speak with

#define NAME_PLACEHOLER_WIDTH 30
#define NAME_PLACEHOLER_HEIGHT 12
#define DIALOGUE_BOX_WIDTH 100
#define DIALOGUE_BOX_HEIGHT 50

static void trigger_dialogue(ecs_entity_t entity);
static void next_sentence(void);
static void end_dialogue(void);
static void process_key_input(void);

static void trigger_dialogue(ecs_entity_t entity)
{
  Dialogue*  dialogue;
  Transform* transform;
  Name*      name;

  _npc = entity;

  dialogue  = ecs_get(g_ecs, entity, DIALOGUE);
  transform = ecs_get(g_ecs, entity, TRANSFORM);
  name      = ecs_get(g_ecs, entity, NAME);

  _box_x = (int)transform->pos.x - DIALOGUE_BOX_WIDTH / 2;
  _box_y = (int)transform->pos.y - DIALOGUE_BOX_HEIGHT - 40;

  if (name != NULL)
    _npc_name = name->value;

  _conversation_name = dialogue->name;

  queue_clear(&_queue);
  for (int i = 0; i < dialogue->sentences->cnt; ++i)
  {
    queue_offer(&_queue, dialogue->sentences->storage[i]);
  }
  keybroad_push_state(process_key_input);
  next_sentence();
}

static void next_sentence(void)
{
  if (!queue_empty(&_queue))
  {
    _current_sentence = queue_poll(&_queue);
    INFO("%s\n", _current_sentence);
  }
  else
  {
    end_dialogue();
  }
}

static void end_dialogue(void)
{
  keybroad_pop_state();
  mediator_broadcast(SYS_SIG_FINISH_CONVERSATION,
                     &(SysEvt_FinishConversation){
                         .npc               = _npc,
                         .npc_name          = _npc_name,
                         .conversation_name = _conversation_name,
                     });
  _current_sentence = NULL;
  _npc_name         = NULL;
  _npc              = ECS_NULL_ENT;
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_A) || key_just_pressed(KEY_B))
  {
    next_sentence();
    Mix_PlayChannel(-1, get_sfx(SFX_BUTTON), 0);
  }
}

static void on_exec_interaction_cmd(pointer_t arg, const SysEvt_ExecInteractionCmd* event)
{
  (void)arg;
  if (strcmp(event->cmd, "TALK") == 0)
  {
    trigger_dialogue(event->entity);
  }
}

void dialogue_system_init()
{
  queue_init(&_queue, 32);
  mediator_connect(SYS_SIG_EXEC_INTERACTION_CMD, NULL, SLOT(on_exec_interaction_cmd));
  _font = get_font(FONT_ITEM_PICKED_UP);
}

void dialogue_system_fini()
{
  queue_fini(&_queue);
}

void dialogue_system_update()
{
  if (_current_sentence != NULL)
  {

    draw_box_w_border(
        &(RECT){
            _box_x - g_viewport.x,
            _box_y - g_viewport.y,
            DIALOGUE_BOX_WIDTH,
            DIALOGUE_BOX_HEIGHT,
        },
        COLOR_BLACK,
        COLOR_WHITE);

    FC_DrawBoxColor(_font,
                    g_renderer,
                    (RECT){
                        _box_x - g_viewport.x + 3,
                        _box_y - g_viewport.y + 10,
                        DIALOGUE_BOX_WIDTH - 6,
                        DIALOGUE_BOX_HEIGHT - 13,
                    },
                    COLOR_WHITE,
                    _current_sentence);
  }
  if (_npc_name != NULL)
  {
    draw_box_w_border(
        &(RECT){
            _box_x - g_viewport.x - 10,
            _box_y - g_viewport.y - 6,
            NAME_PLACEHOLER_WIDTH,
            NAME_PLACEHOLER_HEIGHT,
        },
        COLOR_BLACK,
        COLOR_WHITE);
    FC_DrawBoxColor(_font,
                    g_renderer,
                    (RECT){
                        _box_x - g_viewport.x - 7,
                        _box_y - g_viewport.y - 3,
                        NAME_PLACEHOLER_WIDTH - 6,
                        NAME_PLACEHOLER_HEIGHT - 6,
                    },
                    COLOR_WHITE,
                    _npc_name);
  }
}
