#ifndef UI_QUALITY_H
#define UI_QUALITY_H
#include "toolbox/toolbox.h"
enum
{
  UI_QUALITY_ON_SELECTED,
  UI_QUALITY_NUM_EVENTS,
};
void ui_quality_display(const char* title, u32 initial_val, u32 min_val, u32 max_val);
void ui_quality_draw(void);
void ui_quality_hook(s32 event_id, Callback callback);
#endif // UI_QUALITY_H
