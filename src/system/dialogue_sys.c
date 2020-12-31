#include "dialogue_sys.h"
#include "components.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "mediator.h"
#include "resources.h"
#include "ui_helper.h"
extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

static Queue       _queue;
static const char* _current_sentence;
static FONT*       _font;

static const COLOR _background_color = { 210, 189, 167, 255 };
static const COLOR _border_color     = { 169, 139, 121, 255 };
static const COLOR _text_color       = { 74, 61, 60, 255 };
static int         _box_x;
static int         _box_y;

#define DIALOGUE_BOX_WIDTH 60
#define DIALOGUE_BOX_HEIGHT 40

static void trigger_dialogue(ecs_entity_t entity);
static void next_sentence(void);
static void end_dialogue(void);
static void process_key_input(void);

static void trigger_dialogue(ecs_entity_t entity)
{
  Dialogue*  dialogue;
  Transform* transform;

  dialogue  = ecs_get(g_ecs, entity, DIALOGUE);
  transform = ecs_get(g_ecs, entity, TRANSFORM);

  _box_x = (int)transform->pos.x - DIALOGUE_BOX_WIDTH / 2;
  _box_y = (int)transform->pos.y - DIALOGUE_BOX_HEIGHT - 40;

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
  _current_sentence = NULL;
  keybroad_pop_state();
}

static void process_key_input(void)
{
  if (key_just_pressed(KEY_A))
  {
    next_sentence();
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
        _background_color,
        _border_color);

    FC_DrawBoxColor(_font,
                    g_renderer,
                    (RECT){
                        _box_x - g_viewport.x + 3,
                        _box_y - g_viewport.y + 3,
                        DIALOGUE_BOX_WIDTH - 6,
                        DIALOGUE_BOX_HEIGHT - 6,
                    },
                    _text_color,
                    _current_sentence);
  }
}
