#ifndef SIGNAL_H
#define SIGNAL_H
#include "common.h"
#include <stdint.h>

typedef void (*slot_t)(pointer_t, const pointer_t);
#define SLOT(__func) ((slot_t)__func)

typedef struct Signal     Signal;
typedef struct Dispatcher Dispatcher;

Signal* signal_new();
void    signal_destroy(Signal* sig);

void signal_connect(Signal* sig, pointer_t user_data, funcptr_t func);
void signal_disconnect(Signal* sig, pointer_t func_or_instance);
void signal_emit(Signal* sig, const pointer_t evt);

Dispatcher* dispatcher_new(u32 num_singals);
void        dispatcher_destroy(Dispatcher* dispatcher);

void dispatcher_connect(Dispatcher* dispatcher, int sig, pointer_t user_data, funcptr_t func);
void dispatcher_disconnect(Dispatcher* dispatcher, int sig, pointer_t func_or_instance);
void dispatcher_emit(Dispatcher* dispatcher, int sig, const pointer_t evt);

#endif // SIGNAL_H
