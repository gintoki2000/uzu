#include "dialogue_sys.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "mediator.h"
#include "resources.h"
#include "ui_helper.h"
#include "ui_list.h"

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

static Queue               _queue;
static const char*         _current_sentence;
static FONT*               _font;
static const char*         _npc_name;
static int                 _box_x;
static int                 _box_y;
static ecs_entity_t        _npc = ECS_NULL_ENT; // current npc who player speak with
static const Conversation* _conversation;

#define NAME_PLACEHOLER_WIDTH 30
#define NAME_PLACEHOLER_HEIGHT 12
#define DIALOGUE_BOX_WIDTH 100
#define DIALOGUE_BOX_HEIGHT 50

static void begin_conversation(ecs_entity_t entity);
static void next_sentence(void);
static void end_conversation(const char* response);
static void process_key_input(void);
static void display_responses(void);

//<-------------------------------------event callbacks---------------------------------->//
static void on_exec_interaction_cmd(pointer_t arg, const SysEvt_ExecInteractionCmd* event);
static void on_list_selected(pointer_t arg, const char* item);

//<======================================================================================>//

static void begin_conversation(ecs_entity_t entity)
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

  _conversation = get_conversation(dialogue->conversation_id);

  queue_clear(&_queue);
  for (int i = 0; i < _conversation->sentences->cnt; ++i)
  {
    queue_offer(&_queue, _conversation->sentences->storage[i]);
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
    display_responses();
  }
}

static void end_conversation(const char* response)
{
  keybroad_pop_state();
  mediator_broadcast(SYS_SIG_FINISH_CONVERSATION,
                     &(SysEvt_FinishConversation){
                         .npc               = _npc,
                         .npc_name          = _npc_name,
                         .conversation_name = _conversation->name,
                         .response          = response,
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

static void display_responses(void)
{
  if (_conversation->responses->cnt > 0)
  {
    ui_list_display((const char**)_conversation->responses->storage, _conversation->responses->cnt);
    ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_selected));
  }
  else
  {
    end_conversation(NULL);
  }
}

static void on_exec_interaction_cmd(pointer_t arg, const SysEvt_ExecInteractionCmd* event)
{
  (void)arg;
  if (strcmp(event->cmd, "TALK") == 0)
  {
    begin_conversation(event->entity);
  }
}

static void on_list_selected(pointer_t arg, const char* item)
{
  (void)arg;
  end_conversation(item);
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
