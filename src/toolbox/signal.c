#include "signal.h"
#include <stdlib.h>

struct signal
{
  u32       allocated;
  u32       count;
  Callback* callbacks;
};

struct dispatcher
{
  u32     num_singals;
  Signal* signals;
};

static Signal* signal_init(Signal* signal)
{
  u32 n = 16;
  signal->count = 0;
  signal->allocated = n;
  signal->callbacks = calloc(n, sizeof(Callback));
  return signal;
}

static void signal_finalize(Signal* signal) { free(signal->callbacks); }

static void signal_encap(Signal* signal)
{
  u32 n = signal->allocated * 2;
  signal->callbacks = realloc(signal->callbacks, n * sizeof(Callback));
}

static void signal_grow_if_need(Signal* signal)
{
  if (signal->count == signal->allocated)
  {
    signal_encap(signal);
  }
}

Signal* signal_new()
{
  Signal* signal = malloc(sizeof(Signal));
  signal_init(signal);
  return signal;
}

void signal_destroy(Signal* signal)
{
  signal_finalize(signal);
  free(signal);
}

void signal_connect(Signal* signal, void* user_data, sig_handler_fn_t handler)
{
  signal_grow_if_need(signal);
  signal->callbacks[signal->count++] = CALLBACK_1(user_data, handler);
}

void signal_disconnect(Signal* signal, sig_handler_fn_t handler)
{
  Callback* callbacks = signal->callbacks;
  u32       count = signal->count;
  for (u32 i = 0; i < count; ++i)
  {
    if (callbacks[i].func == (pointer_t)handler)
    {
      callbacks[i] = callbacks[count - 1];
      --signal->count;
      return;
    }
  }
}

void signal_emit(Signal* signal, const void* evt)
{
  sig_handler_fn_t func;
  pointer_t        user_data;
  for (u32 i = 0; i < signal->count; ++i)
  {
    func = (sig_handler_fn_t)signal->callbacks[i].func;
    user_data = signal->callbacks[i].user_data;

    func(user_data, evt);
  }
}

static Dispatcher* dispatcher_init(Dispatcher* dispatcher, u32 num_singals)
{
  dispatcher->num_singals = num_singals;
  dispatcher->signals = calloc(num_singals, sizeof(Signal));
  return dispatcher;
}

static void dispatcher_finalize(Dispatcher* dispatcher)
{
  for (u32 i = 0; i < dispatcher->num_singals; ++i)
  {
    signal_finalize(dispatcher->signals + i);
  }
}

Dispatcher* dispatcher_new(u32 num_singals)
{
  Dispatcher* dispatcher = malloc(sizeof(Dispatcher));
  dispatcher_init(dispatcher, num_singals);
  return dispatcher;
}

void dispatcher_destroy(Dispatcher* dispatcher)
{
  dispatcher_finalize(dispatcher);
  free(dispatcher);
}

void dispatcher_connect(Dispatcher*      dispatcher,
                        int              signal,
                        void*            user_data,
                        sig_handler_fn_t handler)
{
  signal_connect(dispatcher->signals + signal, user_data, handler);
}

void dispatcher_disconnect(Dispatcher* dispatcher, int signal, sig_handler_fn_t handler)
{
  signal_disconnect(dispatcher->signals + signal, handler);
}

void dispatcher_emit(Dispatcher* dispatcher, int signal, const void* evt)
{
  signal_emit(dispatcher->signals + signal, evt);
}
