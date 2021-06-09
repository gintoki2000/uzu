#include "toolbox/signal.h"
#include <stdlib.h>

#define SIGNAL_DEFAULT_CAPACITY 16



struct Emitter
{
  u32     numSingals;
  Signal* signals;
};

Signal* signal_init(Signal* self)
{
  self->count        = 0;
  self->size        = SIGNAL_DEFAULT_CAPACITY;
  self->slots      = SDL_calloc(SIGNAL_DEFAULT_CAPACITY, sizeof(Callback));
  self->isEmiting = FALSE;
  return self;
}

void signal_fini(Signal* sig)
{
  SDL_free(sig->slots);
}

Signal* signal_create()
{
  Signal* sig = malloc(sizeof(Signal));
  signal_init(sig);
  return sig;
}

void signal_free(Signal* sig)
{
  if (sig)
  {
    signal_fini(sig);
    SDL_free(sig);
  }
}

static BOOL signal_contains(Signal* self, Callback callback)
{
  Callback* slots = self->slots;
  for (int i = self->count - 1; i >= 0; --i)
    if (callback.userData == slots[i].userData && callback.func == slots[i].func)
      return TRUE;
  return FALSE;
}

void signal_connect(Signal* self, Callback callback)
{
  if (signal_contains(self, callback))
    return;
  if (self->count == self->size)
  {
    self->size *= 2;
    self->slots = SDL_realloc(self->slots, self->size * sizeof(Callback));
  }
  self->slots[self->count++] = callback;
}

BOOL signal_disconnect(Signal* self, void (*func)())
{
  Callback* slots = self->slots;
  for (int i = self->count - 1; i >= 0; --i)
    if (slots[i].userData == NULL && slots[i].func == func)
    {
      slots[i] = slots[--self->count];
      return TRUE;
    }
  return FALSE;
}

BOOL signal_disconnect_ex(Signal* self, void* instance, void (*func)())
{
  Callback* slots = self->slots;
  for (int i = self->count - 1; i >= 0; --i)
    if (instance == slots[i].userData && func == slots[i].func)
    {
      slots[i] = slots[--self->count];
      return TRUE;
    }
  return FALSE;
}

void signal_emit(Signal* self, const void* data)
{
  ASSERT(!self->isEmiting);
  self->isEmiting = TRUE;
  for (int i = self->count - 1; i >= 0; --i)
  {
    INVOKE_EVENT(self->slots[i], data);
  }
  self->isEmiting = FALSE;
}

static Emitter* emitter_init(Emitter* self, u32 num_singals)
{
  self->numSingals = num_singals;
  self->signals     = SDL_calloc(num_singals, sizeof(Signal));
  for (u32 i = 0; i < num_singals; ++i)
  {
    signal_init(&self->signals[i]);
  }
  return self;
}

static void emitter_fini(Emitter* self)
{
  for (u32 i = 0; i < self->numSingals; ++i)
  {
    signal_fini(self->signals + i);
  }
  SDL_free(self->signals);
}

Emitter* emitter_create(u32 num_singals)
{
  Emitter* self = malloc(sizeof(Emitter));
  emitter_init(self, num_singals);
  return self;
}

void emitter_free(Emitter* self)
{
  if (self)
  {
    emitter_fini(self);
    SDL_free(self);
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
