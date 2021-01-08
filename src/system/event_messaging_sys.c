#include "event_messaging_sys.h"

static Dispatcher* _dispatcher;

void ems_init()
{
  _dispatcher = dispatcher_new(NUM_MSGS);
}

void ems_fini()
{
  dispatcher_destroy(_dispatcher);
  _dispatcher = NULL;
}

void ems_connect(int sig_id, pointer_t udata, funcptr_t func)
{
  dispatcher_connect(_dispatcher, sig_id, udata, func);
}

void ems_broadcast(int sig_id, const pointer_t event)
{
  dispatcher_emit(_dispatcher, sig_id, event);
}

void ems_disconnect(int sig_id, pointer_t func)
{
  dispatcher_disconnect(_dispatcher, sig_id, func);
}
