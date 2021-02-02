#ifndef UI_DIALOGUE_H
#define UI_DIALOGUE_H
#include "toolbox/toolbox.h"
enum
{
  UI_DIALOGUE_EVENT_NEXT_SENTENCE,
  UI_DIALOGUE_EVENT_DIALOGUE_FINISHED,
  UI_DIALOGUE_NUM_EVENTS
};

typedef struct UI_DialogueEvent_DialogueFinished
{
  const char* name;
  const char* response;
} UI_DialogueEvent_DialogueFinished;

typedef struct UI_DialogueEvent_NextSentence
{
  const char* sentence;
} UI_DialogueEvent_NextSentence;

void ui_dialogue_init(void);
void ui_dialogue_fini(void);
void ui_dialogue_show(u16 dialogue_id);
void ui_dialogue_hook(u16 event_id, Callback callback);
void ui_dialogue_update(void);
void ui_dialogue_draw(void);
#endif // UI_DIALOGUE_H
