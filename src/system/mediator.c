#include "mediator.h"

static Dispatcher* _dispatcher;

void mediator_init()
{
  _dispatcher = dispatcher_new(NUM_SYSTEM_SIGNALS);
}

void mediator_fini()
{
  dispatcher_destroy(_dispatcher);
  _dispatcher = NULL;
}

void mediator_connect(int sig_id, pointer_t udata, slot_t sig_handler)
{
  dispatcher_connect(_dispatcher, sig_id, udata, sig_handler);
}

void mediator_broadcast(int sig_id, const pointer_t event)
{
  dispatcher_emit(_dispatcher, sig_id, event);
}

void mediator_disconnect(int sig_id, pointer_t func)
{
  dispatcher_disconnect(_dispatcher, sig_id, (slot_t)func);
}
