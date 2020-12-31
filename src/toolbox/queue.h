#ifndef QUEUE_H
#define QUEUE_H
#include "common.h"
typedef struct
{
  pointer_t*   storage;
  u32          cnt;
  u32          cap;
  s32          front;
  s32          rear;
  destroy_fn_t destroy_fn;
} Queue;

Queue* queue_init_full(Queue* self, u32 cap, destroy_fn_t destroy_fn);
Queue* queue_init(Queue* self, u32 cap);
void   queue_fini(Queue* self);

void      queue_offer(Queue* self, pointer_t item);
pointer_t queue_poll(Queue* self);
pointer_t queue_peek(Queue* self);
void      queue_clear(Queue* self);

INLINE BOOL queue_empty(Queue* self)
{
  return self->cnt == 0;
}

INLINE BOOL queue_full(Queue* self)
{
  return self->cnt == self->cap;
}

#endif // QUEUE_H
