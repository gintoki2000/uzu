#ifndef SIGNAL_H
#define SIGNAL_H
#include <stdint.h>
#include "common.h"

typedef void (*sig_handler_fn_t)(void*, const void*);

typedef struct signal     Signal;
typedef struct dispatcher Dispatcher;

Signal* signal_new();
void    signal_destroy(Signal* signal);

void signal_connect(Signal* signal, void* user_data, sig_handler_fn_t handler);
void signal_disconnect(Signal* signal, sig_handler_fn_t handler);
void signal_emit(Signal* signal, const void* evt);

Dispatcher* dispatcher_new(u32 num_singals);
void        dispatcher_destroy(Dispatcher* dispatcher);

void dispatcher_connect(Dispatcher*      dispatcher,
                        int              signal,
                        void*            user_data,
                        sig_handler_fn_t handler);
void dispatcher_disconnect(Dispatcher* dispatcher, int signal, sig_handler_fn_t handler);
void dispatcher_emit(Dispatcher* dispatcher, int signal, const void* evt);

#endif // SIGNAL_H
