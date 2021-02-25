#ifndef UI_LIST_H
#define UI_LIST_H
#include "toolbox/toolbox.h"

enum
{
  UI_LIST_ON_CLOSE,
  UI_LIST_ON_SELECT,
  UI_LIST_NUM_EVENTS,
};

#define UI_LIST_POS_CENTER_X (-1)
#define UI_LIST_POS_CENTER_Y (-1)

#define UI_LIST_ENTRY_MAX_LEN (50)
typedef struct UI_List_Entry
{
  char text[UI_LIST_ENTRY_MAX_LEN + 1];
} UI_List_Entry;

void ui_list_display(const char* items[], u32 cnt);
void ui_list_close(void);
void ui_list_hook(u32 event_id, Callback callback);
void ui_list_draw(void);
void ui_list_set_pos(s32 x, s32 y);

#endif // UI_LIST_H
