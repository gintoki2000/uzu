#ifndef UI_LIST_H
#define UI_LIST_H
#include "toolbox/toolbox.h"

extern BOOL g_ui_list_visible;
extern int  g_ui_list_pos_x;
extern int  g_ui_list_pos_y;

enum
{
  UI_LIST_ON_CLOSE,
  UI_LIST_ON_SELECT,
  UI_LIST_NUM_EVENTS,
};

void ui_list_display(const char* items[], u32 cnt);
void ui_list_close(void);
void ui_list_hook(u32 event_id, Callback callback);

void ui_list_draw(void);

#endif // UI_LIST_H
