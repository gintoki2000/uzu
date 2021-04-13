#include "system/event_messaging_sys.h"

static Emitter* _dispatcher;

void ems_init()
{
  _dispatcher = emitter_new(NUM_MSGS);
}

void ems_fini()
{
  emitter_delete(_dispatcher);
  _dispatcher = NULL;
}

void ems_connect(int signal, Callback callback)
{
  emitter_connect(_dispatcher, signal, callback);
}

void ems_broadcast(int signal, const void* event)
{
  emitter_emit(_dispatcher, signal, event);
}

void ems_disconnect(int signal, void (*fn)())
{
  emitter_disconnect(_dispatcher, signal, fn);
}
