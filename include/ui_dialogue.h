#ifndef UI_DIALOGUE_H
#define UI_DIALOGUE_H
#include "toolbox/toolbox.h"

void ui_dialogue_init(void);
void ui_dialogue_set_visible(BOOL value);
void ui_dialogue_set_delay(u16 value);
void ui_dialogue_set_text(const char* text);
void ui_dialogue_update(void);
void ui_dialogue_draw(void);
#endif // UI_DIALOGUE_H
