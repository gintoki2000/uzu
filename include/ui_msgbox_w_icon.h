#ifndef UI_MSGBOX_W_ICON_H
#define UI_MSGBOX_W_ICON_H
#define UI_MSGBOX_W_ICON_MAX_TEXT_LEN 50
#include "global.h"
typedef struct UI_MsgBoxWIcon_Entry
{
  char text[UI_MSGBOX_W_ICON_MAX_TEXT_LEN + 1];
  Icon icon;
} UI_MsgBoxWIcon_Entry;

void ui_msgbox_w_icon_init(void);
void ui_msgbox_w_icon_fini(void);
void ui_msgbox_w_icon_draw(void);
void ui_msgbox_w_icon_update(void);
void ui_msgbox_w_icon_show(const UI_MsgBoxWIcon_Entry* entries, u16 nentry);
#endif // UI_MSGBOX_W_ICON_H
