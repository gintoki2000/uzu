#include "signal.h"
#include <stdlib.h>

#define SIGNAL_DEFAULT_CAPACITY 16

struct Signal
{
  u32       cap;
  u32       cnt;
  Callback* slots;
};

struct Dispatcher
{
  u32     num_singals;
  Signal* signals;
};

static Signal* signal_init(Signal* sig)
{
  sig->cnt   = 0;
  sig->cap   = SIGNAL_DEFAULT_CAPACITY;
  sig->slots = calloc(SIGNAL_DEFAULT_CAPACITY, sizeof(Callback));
  return sig;
}

static void signal_fini(Signal* sig)
{
  free(sig->slots);
}

static void signal_encap(Signal* sig)
{
  sig->cap *= 2;
  sig->slots = realloc(sig->slots, sig->cap * sizeof(Callback));
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

void signal_connect(Signal* sig, pointer_t user_data, funcptr_t func)
{
  if (sig->cnt == sig->cap)
  {
    signal_encap(sig);
  }
  sig->slots[sig->cnt++] = CALLBACK_1(user_data, func);
}

void signal_disconnect(Signal* sig, pointer_t func_or_instance)
{
  Callback* slots = sig->slots;
  u32       cnt   = sig->cnt;
  for (u32 i = 0; i < cnt; ++i)
  {
    if (slots[i].func == (void (*)())func_or_instance || slots[i].user_data == func_or_instance)
    {
      slots[i] = slots[cnt - 1];
      --sig->cnt;
      return;
    }
  }
}

void signal_emit(Signal* sig, const pointer_t evt)
{
  for (u32 i = 0; i < sig->cnt; ++i)
  {
    INVOKE_CALLBACK(sig->slots[i], void, evt);
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

static void dispatcher_fini(Dispatcher* dp)
{
  for (u32 i = 0; i < dp->num_singals; ++i)
  {
    signal_fini(dp->signals + i);
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
    dispatcher_fini(dp);
    free(dp);
  }
}

void dispatcher_connect(Dispatcher* dp, int sig, pointer_t user_data, funcptr_t func)
{
  signal_connect(dp->signals + sig, user_data, func);
}

void dispatcher_disconnect(Dispatcher* dp, int sig, pointer_t func_or_instance)
{
  signal_disconnect(dp->signals + sig, func_or_instance);
}

void dispatcher_emit(Dispatcher* dp, int sig, const pointer_t evt)
{
  signal_emit(dp->signals + sig, evt);
}
