#ifndef UI_H
#define UI_H
#include "toolbox/toolbox.h"
struct UI
{
  BOOL     is_visible;
  Emitter* emitter;
  void (*clean_up)(void* self);
  void (*draw)(void* self, RENDERER*);
  void (*on_show)(void* self);
  void (*on_hide)(void* self);
};

#define UI(ptr) ((struct UI*)ptr)

void ui_delete(struct UI* ui);
void ui_init(struct UI* self, int num_signals);
void ui_set_visible(struct UI* self, BOOL value);
void ui_connect(struct UI* self, u16 signal, Callback callback);
void ui_disconnect_function(struct UI* self, u16 signal, void (*fn)());
void ui_emit_signal(struct UI* ui, u16 signal, const void* data);
void _ui_cleanup(void* self);
void _ui_on_show(void* self);
void _ui_on_hide(void* self);

void ui_add(struct UI* ui_component);
void ui_draw(RENDERER* renderer);
#endif // UI_H
