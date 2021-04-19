#include "toolbox/signal.h"
#include <stdlib.h>

#define SIGNAL_DEFAULT_CAPACITY 16

struct Signal
{
  int       cap;
  int       cnt;
  Callback* slots;
  BOOL      is_emiting;
};

struct Emitter
{
  u32     num_singals;
  Signal* signals;
};

static Signal* signal_init(Signal* sig)
{
  sig->cnt        = 0;
  sig->cap        = SIGNAL_DEFAULT_CAPACITY;
  sig->slots      = calloc(SIGNAL_DEFAULT_CAPACITY, sizeof(Callback));
  sig->is_emiting = FALSE;
  return sig;
}

static void signal_fini(Signal* sig)
{
  free(sig->slots);
}

Signal* signal_new()
{
  Signal* sig = malloc(sizeof(Signal));
  signal_init(sig);
  return sig;
}

void signal_destroy(Signal* sig)
{
  if (sig)
  {
    signal_fini(sig);
    free(sig);
  }
}

void signal_connect(Signal* self, Callback callback)
{
  if (self->cnt == self->cap)
  {
    self->cap *= 2;
    self->slots = realloc(self->slots, self->cap * sizeof(Callback));
  }
  self->slots[self->cnt++] = callback;
}

void signal_disconnect(Signal* self, void (*fn)())
{
  for (int i = self->cnt - 1; i >= 0; --i)
    while (i < self->cnt && self->cnt > 0 && self->slots[i].func == fn)
      self->slots[i] = self->slots[--self->cnt];
}

void signal_disconnect_ex(Signal* self, void* instance, void (*fn)())
{
  BOOL func_matched;
  for (int i = self->cnt - 1; i >= 0; --i)
  {
    func_matched = fn != NULL ? fn == self->slots[i].func : TRUE;
    while (i < self->cnt && self->cnt > 0 && self->slots[i].user_data == instance && func_matched)
      self->slots[i] = self->slots[--self->cnt];
  }
}

void signal_emit(Signal* self, const void* data)
{
  ASSERT(!self->is_emiting);
  self->is_emiting = TRUE;
  for (int i = self->cnt - 1; i >= 0; --i)
  {
    INVOKE_EVENT(self->slots[i], data);
  }
  self->is_emiting = FALSE;
}

static Emitter* emitter_init(Emitter* self, u32 num_singals)
{
  self->num_singals = num_singals;
  self->signals     = calloc(num_singals, sizeof(Signal));
  for (u32 i = 0; i < num_singals; ++i)
  {
    signal_init(&self->signals[i]);
  }
  return self;
}

static void emitter_fini(Emitter* self)
{
  for (u32 i = 0; i < self->num_singals; ++i)
  {
    signal_fini(self->signals + i);
  }
  free(self->signals);
}

Emitter* emitter_new(u32 num_singals)
{
  Emitter* self = malloc(sizeof(Emitter));
  emitter_init(self, num_singals);
  return self;
}

void emitter_delete(Emitter* self)
{
  if (self)
  {
    emitter_fini(self);
    free(self);
  }
}

void emitter_connect(Emitter* self, int sig, Callback callback)
{
  signal_connect(self->signals + sig, callback);
}

void emitter_disconnect(Emitter* self, int sig, void (*fn)())
{
  signal_disconnect(self->signals + sig, fn);
}

void emitter_emit(Emitter* self, int sig, const void* data)
{
  signal_emit(self->signals + sig, data);
}

void emitter_disconnect_ex(Emitter* self, int sig, void* instance, void (*fn)())
{
  signal_disconnect_ex(&self->signals[sig], instance, fn);
}
