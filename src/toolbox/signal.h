#ifndef SIGNAL_H
#define SIGNAL_H
#include "common.h"
#include <stdint.h>

typedef void (*sig_handler_fn_t)(void*, const void*);

typedef struct Signal     Signal;
typedef struct Dispatcher Dispatcher;

Signal* signal_new();
void    signal_destroy(Signal* sig);

void signal_connect(Signal* sig, void* user_data, sig_handler_fn_t handler);
void signal_disconnect(Signal* sig, sig_handler_fn_t handler);
void signal_emit(Signal* sig, const void* evt);

Dispatcher* dispatcher_new(u32 num_singals);
void        dispatcher_destroy(Dispatcher* dispatcher);

void dispatcher_connect(Dispatcher* dispatcher, int sig, void* user_data, sig_handler_fn_t handler);
void dispatcher_disconnect(Dispatcher* dispatcher, int sig, sig_handler_fn_t handler);
void dispatcher_emit(Dispatcher* dispatcher, int sig, const void* evt);

#endif // SIGNAL_H
