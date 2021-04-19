#include "ui.h"

void ui_delete(struct UI* ui)
{
  if (ui != NULL)
  {
    ui->clean_up(ui);
    SDL_free(ui);
  }
}

void ui_init(struct UI* self, int num_events)
{
  self->clean_up = _ui_cleanup;
  self->on_hide  = _ui_on_hide;
  self->on_show  = _ui_on_show;

  self->is_visible = FALSE;
  self->emitter    = emitter_new(num_events);
}

void _ui_cleanup(void* self)
{
  emitter_delete(UI(self)->emitter);
}

void _ui_on_hide(SDL_UNUSED void* self)
{
}

void _ui_on_show(SDL_UNUSED void* self)
{
}

void ui_set_visible(struct UI* self, BOOL value)
{
  if (self->is_visible != value)
  {
    self->is_visible = value;
    value ? self->on_show(self) : self->on_hide(self);
  }
}

void ui_emit_event(struct UI* ui, u16 event, const void* data)
{
  emitter_emit(ui->emitter, event, data);
}

void ui_connect(struct UI* self, u16 event, Callback callback)
{
  emitter_connect(self->emitter, event, callback);
}

void ui_disconnect_function(struct UI* self, u16 event, void (*fn)())
{
  emitter_disconnect(self->emitter, event, fn);
}
