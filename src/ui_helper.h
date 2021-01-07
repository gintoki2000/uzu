#ifndef UI_HELPER_H
#define UI_HELPER_H
#include "toolbox/toolbox.h"
void draw_box_w_border(const RECT* rect, COLOR background_color, COLOR border_color);

#define draw_textbox_color(__font_id, __renderer, __rect, __color, __fmt, ...)                     \
  FC_DrawBoxColor(get_font(__font_id), __renderer, __rect, __fmt, __VA_ARGS__)
#define draw_textbox_ex(__font_id, __renderer, __rect, __effect, __fmt, ...)                       \
  FC_DrawBoxEffect(get_font(__font_id), __renderer, __rect, __effect, __fmt, __VA_ARGS__)

#endif // UI_HELPER_H
