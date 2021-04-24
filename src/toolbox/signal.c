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

static BOOL signal_contains(Signal* self, Callback callback)
{
  Callback* slots = self->slots;
  for (int i = self->cnt - 1; i >= 0; --i)
    if (callback.user_data == slots[i].user_data && callback.func == slots[i].func)
      return TRUE;
  return FALSE;
}

void signal_connect(Signal* self, Callback callback)
{
  if (signal_contains(self, callback))
    return;
  if (self->cnt == self->cap)
  {
    self->cap *= 2;
    self->slots = realloc(self->slots, self->cap * sizeof(Callback));
  }
  self->slots[self->cnt++] = callback;
}

BOOL signal_disconnect(Signal* self, void (*func)())
{
  Callback* slots = self->slots;
  for (int i = self->cnt - 1; i >= 0; --i)
    if (slots[i].user_data == NULL && slots[i].func == func)
    {
      slots[i] = slots[--self->cnt];
      return TRUE;
    }
  return FALSE;
}

BOOL signal_disconnect_ex(Signal* self, void* instance, void (*func)())
{
  Callback* slots = self->slots;
  for (int i = self->cnt - 1; i >= 0; --i)
    if (instance == slots[i].user_data && func == slots[i].func)
    {
      slots[i] = slots[--self->cnt];
      return TRUE;
    }
  return FALSE;
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
  signal_connect(&self->signals[sig], callback);
}

void emitter_disconnect(Emitter* self, int sig, void (*func)())
{
  signal_disconnect(&self->signals[sig], func);
}

void emitter_emit(Emitter* self, int sig, const void* data)
{
  signal_emit(&self->signals[sig], data);
}

void emitter_disconnect_ex(Emitter* self, int sig, void* instance, void (*func)())
{
  signal_disconnect_ex(&self->signals[sig], instance, func);
}
