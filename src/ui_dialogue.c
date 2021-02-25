#include "ui_dialogue.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "resources.h"
#include "global.h"
#include "ui_list.h"

#define MAX_TEXT_LENGTH 255

static Callback            _events[UI_DIALOGUE_NUM_EVENTS];
static Queue               _queue;
static const char*         _current_sentence;
static FONT*               _font;
static const char*         _npc_name;
static const Conversation* _conversation;
static TEXTURE*            _texture_dialogue;
static u16                 _sentence_length;
static char                _displayed_text[MAX_TEXT_LENGTH + 1];
static u16                 _displayed_text_length;
static u16                 _delay; // thời gian cho kí tự tiếp theo xuất hiện
static u16  _elpased_time;         // thời gian kể từ khi kí tự trước đó xuất hiện
static BOOL _is_visible;

static const RECT _rect_dialogue = { 10, 167, 299, 60 };
static const RECT _rect_textbox  = { 10 + 17, 167 + 5, 282, 54 };

static void next_sentence(void);
static void process_input(void*);
static void display_responses(void);
static void end_conversation(const char* response);
static void fire_event(u16 event_id, const void* data);

extern RENDERER* g_renderer;

static void on_list_selected(pointer_t arg, const char* item);

void ui_dialogue_init(void)
{
  queue_init(&_queue, 32);
  _font             = get_font(FONT_DAMAGE_INDICATOR);
  _texture_dialogue = get_texture(TEX_UI_DIALOGUE);
  _is_visible       = FALSE;
  _delay            = 3;
  SDL_strlcpy(_displayed_text, "", MAX_TEXT_LENGTH);
}

void ui_dialogue_fini(void)
{
  queue_fini(&_queue);
}

void ui_dialogue_show(u16 conversation_id)
{
  _is_visible   = TRUE;
  _conversation = get_conversation(conversation_id);
  queue_clear(&_queue);
  for (int i = 0; i < _conversation->sentences->cnt; ++i)
    queue_offer(&_queue, _conversation->sentences->storage[i]);
  next_sentence();
  input_push_state(INPUT_STATE_INST_1(process_input));
}

void ui_dialogue_hook(u16 event_id, Callback callback)
{
  _events[event_id] = callback;
}

void ui_dialogue_update(void)
{
  if (_is_visible)
  {
    if (++_elpased_time >= _delay)
    {
      _elpased_time = 0;
      if (_displayed_text_length < MAX_TEXT_LENGTH && _displayed_text_length < _sentence_length)
      {
        _displayed_text[_displayed_text_length]     = _current_sentence[_displayed_text_length];
        _displayed_text[_displayed_text_length + 1] = '\0';
        ++_displayed_text_length;
      }
    }
  }
}

void ui_dialogue_draw(void)
{
  if (!_is_visible)
    return;
  SDL_RenderCopy(g_renderer, _texture_dialogue, NULL, &_rect_dialogue);
  FC_DrawBoxColor(_font, g_renderer, _rect_textbox, COLOR_BLACK, "%s", _displayed_text);
}

static void process_input(SDL_UNUSED void* arg)
{
  if (button_just_pressed(BUTTON_INTERACT))
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

static void end_conversation(const char* response)
{
  input_pop_state();
  fire_event(UI_DIALOGUE_EVENT_DIALOGUE_FINISHED,
             &(UI_DialogueEvent_DialogueFinished){ _conversation->name, response });
  _current_sentence = NULL;
  _npc_name         = NULL;
  _is_visible       = FALSE;
}

static void next_sentence(void)
{
  if (!queue_empty(&_queue))
  {
    _current_sentence = queue_poll(&_queue);
    fire_event(UI_DIALOGUE_EVENT_NEXT_SENTENCE, _current_sentence);
    INFO("%s\n", _current_sentence);
    _sentence_length       = strlen(_current_sentence);
    _displayed_text_length = 0;
    _elpased_time          = 0;
    _displayed_text[0]     = '\0';
  }
  else
  {
    display_responses();
  }
}

static void on_list_selected(pointer_t arg, const char* item)
{
  (void)arg;
  end_conversation(item);
}

static void fire_event(u16 event_id, const void* data)
{
  if (_events[event_id].func != NULL)
    INVOKE_CALLBACK(_events[event_id], void, data);
}
