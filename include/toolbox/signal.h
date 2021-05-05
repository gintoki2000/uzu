#ifndef SIGNAL_H
#define SIGNAL_H
#include "common.h"

struct Signal
{
  int       cap;
  int       cnt;
  Callback* slots;
  BOOL      is_emiting;
};

typedef struct Signal  Signal;
typedef struct Emitter Emitter;

Signal* signal_new();
void    signal_destroy(Signal* self);

Signal* signal_init(Signal* self);
void    signal_fini(Signal* self);
void    signal_connect(Signal* self, Callback callback);
BOOL    signal_disconnect(Signal* self, void (*fn)());
BOOL    signal_disconnect_ex(Signal* self, void* instance, void (*fn)());
void    signal_emit(Signal* self, const void* data);

Emitter* emitter_new(u32 num_singals);
void     emitter_delete(Emitter* emitter);
void     emitter_connect(Emitter* self, int sig, Callback callback);
void     emitter_disconnect(Emitter* self, int sig, void (*fn)());
void     emitter_disconnect_ex(Emitter* self, int sig, void* instance, void (*fn)());
void     emitter_emit(Emitter* self, int sig, const void* data);

#endif // SIGNAL_H
