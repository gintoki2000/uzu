#ifndef UI_MSGBOX_H
#define UI_MSGBOX_H
#include "toolbox/toolbox.h"
void ui_msgbox_display(const char* msg);
void ui_msgbox_display_timed(const char* msg, s32 ticks);
void ui_msgbox_draw(void);
#endif // UI_MSGBOX_H
