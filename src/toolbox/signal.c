#include "signal.h"
#include <stdlib.h>

struct Signal
{
  u32       allocated;
  u32       count;
  Callback* callbacks;
};

struct Dispatcher
{
  u32     num_singals;
  Signal* signals;
};

static Signal* signal_init(Signal* sig)
{
  u32 n          = 16;
  sig->count     = 0;
  sig->allocated = n;
  sig->callbacks = calloc(n, sizeof(Callback));
  return sig;
}

static void signal_finalize(Signal* sig) { free(sig->callbacks); }

static void signal_encap(Signal* sig)
{
  sig->allocated *= 2;
  sig->callbacks = realloc(sig->callbacks, sig->allocated * sizeof(Callback));
}

static void signal_grow_if_need(Signal* sig)
{
  if (sig->count == sig->allocated)
  {
    signal_encap(sig);
  }
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
    signal_finalize(sig);
    free(sig);
  }
}

void signal_connect(Signal* sig, void* user_data, slot_t slot)
{
  signal_grow_if_need(sig);
  sig->callbacks[sig->count++] = CALLBACK_1(user_data, slot);
}

void signal_disconnect(Signal* sig, slot_t slot)
{
  Callback* callbacks = sig->callbacks;
  u32       count     = sig->count;
  for (u32 i = 0; i < count; ++i)
  {
    if (callbacks[i].func == (pointer_t)slot)
    {
      callbacks[i] = callbacks[count - 1];
      --sig->count;
      return;
    }
  }
}

void signal_emit(Signal* sig, const void* evt)
{
  slot_t    func;
  pointer_t user_data;
  for (u32 i = 0; i < sig->count; ++i)
  {
    func      = (slot_t)sig->callbacks[i].func;
    user_data = sig->callbacks[i].user_data;

    func(user_data, evt);
  }
}

static Dispatcher* dispatcher_init(Dispatcher* dp, u32 num_singals)
{
  dp->num_singals = num_singals;
  dp->signals     = calloc(num_singals, sizeof(Signal));
  for (u32 i = 0; i < num_singals; ++i)
  {
    signal_init(&dp->signals[i]);
  }
  return dp;
}

static void dispatcher_finalize(Dispatcher* dp)
{
  for (u32 i = 0; i < dp->num_singals; ++i)
  {
    signal_finalize(dp->signals + i);
  }
}

Dispatcher* dispatcher_new(u32 num_singals)
{
  Dispatcher* dp = malloc(sizeof(Dispatcher));
  dispatcher_init(dp, num_singals);
  return dp;
}

void dispatcher_destroy(Dispatcher* dp)
{
  if (dp)
  {
    dispatcher_finalize(dp);
    free(dp);
  }
}

void dispatcher_connect(Dispatcher* dp, int sig, void* user_data, slot_t slot)
{
  signal_connect(dp->signals + sig, user_data, slot);
}

void dispatcher_disconnect(Dispatcher* dp, int sig, slot_t slot)
{
  signal_disconnect(dp->signals + sig, slot);
}

void dispatcher_emit(Dispatcher* dp, int sig, const void* evt)
{
  signal_emit(dp->signals + sig, evt);
}
