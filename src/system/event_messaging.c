#include "system/event_messaging_sys.h"

static Emitter* _emitter;

void ems_init()
{
  _emitter = emitter_create(NUM_MSGS);
}

void ems_fini()
{
  emitter_free(_emitter);
  _emitter = NULL;
}

void ems_connect(int signal, Callback callback)
{
  emitter_connect(_emitter, signal, callback);
}

void ems_broadcast(int signal, const void* event)
{
  emitter_emit(_emitter, signal, event);
}

void ems_disconnect(int signal, Func fn)
{
  emitter_disconnect(_emitter, signal, fn);
}

void ems_disconnect_ex(int signal, void* instance, Func fn)
{
  emitter_disconnect_ex(_emitter, signal, instance, fn);
}
