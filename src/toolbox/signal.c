#include "toolbox/signal.h"
#include <stdlib.h>

#define SIGNAL_DEFAULT_CAPACITY 16

struct Signal
{
  u32       cap;
  u32       cnt;
  Callback* slots;
  u32       disconnect_buff_cap;
  u32       disconnect_buff_cnt;
  void (**disconnect_buff)();
  BOOL is_emiting;
};

struct Emitter
{
  u32     num_singals;
  Signal* signals;
};

static Signal* signal_init(Signal* sig)
{
  sig->cnt                 = 0;
  sig->cap                 = SIGNAL_DEFAULT_CAPACITY;
  sig->slots               = calloc(SIGNAL_DEFAULT_CAPACITY, sizeof(Callback));
  sig->disconnect_buff     = calloc(SIGNAL_DEFAULT_CAPACITY, sizeof(void*));
  sig->disconnect_buff_cnt = 0;
  sig->disconnect_buff_cap = SIGNAL_DEFAULT_CAPACITY;
  sig->is_emiting          = FALSE;
  return sig;
}

static void signal_fini(Signal* sig)
{
  free(sig->disconnect_buff);
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

void signal_connect(Signal* sig, Callback callback)
{
  if (sig->cnt == sig->cap)
  {
    sig->cap *= 2;
    sig->slots = realloc(sig->slots, sig->cap * sizeof(Callback));
  }
  sig->slots[sig->cnt++] = callback;
}

static void signal_internal_disconnect(Signal* sig, void (*fn)())
{
  Callback* slots = sig->slots;
  int       cnt   = sig->cnt;

  for (int i = cnt - 1; i >= 0; --i)
  {
    if (slots[i].func == fn)
    {
      slots[i] = slots[sig->cnt - 1];
      --sig->cnt;
    }
  }
}

void signal_disconnect(Signal* self, void (*fn)())
{
  if (self->is_emiting)
  {
    if (self->disconnect_buff_cnt == self->disconnect_buff_cap)
    {
      self->disconnect_buff_cap *= 2;
      self->disconnect_buff = realloc(self->disconnect_buff, self->disconnect_buff_cap);
    }
    self->disconnect_buff[self->disconnect_buff_cnt++] = fn;
  }
  else
  {
    signal_internal_disconnect(self, fn);
  }
}

static void signal_do_removing(Signal* sig)
{
  for (u32 i = 0; i < sig->disconnect_buff_cnt; ++i)
  {
    signal_internal_disconnect(sig, sig->disconnect_buff[i]);
  }
  sig->disconnect_buff_cnt = 0;
}

void signal_emit(Signal* sig, const void* evt)
{
  for (u32 i = 0; i < sig->cnt; ++i)
  {
    INVOKE_CALLBACK(sig->slots[i], void, evt);
  }
  signal_do_removing(sig);
}

static Emitter* dispatcher_init(Emitter* dp, u32 num_singals)
{
  dp->num_singals = num_singals;
  dp->signals     = calloc(num_singals, sizeof(Signal));
  for (u32 i = 0; i < num_singals; ++i)
  {
    signal_init(&dp->signals[i]);
  }
  return dp;
}

static void emitter_fini(Emitter* dp)
{
  for (u32 i = 0; i < dp->num_singals; ++i)
  {
    signal_fini(dp->signals + i);
  }
  free(dp->signals);
}

Emitter* emitter_new(u32 num_singals)
{
  Emitter* dp = malloc(sizeof(Emitter));
  dispatcher_init(dp, num_singals);
  return dp;
}

void emitter_delete(Emitter* dp)
{
  if (dp)
  {
    emitter_fini(dp);
    free(dp);
  }
}

void emitter_connect(Emitter* dp, int sig, Callback callback)
{
  signal_connect(dp->signals + sig, callback);
}

void emitter_disconnect(Emitter* dp, int sig, void (*fn)())
{
  signal_disconnect(dp->signals + sig, fn);
}

void emitter_emit(Emitter* dp, int sig, const void* evt)
{
  signal_emit(dp->signals + sig, evt);
}
