#include "toolbox/queue.h"

Queue* queue_init_full(Queue* self, u32 cap, DestroyFunc destroyFunc)
{
  self->cnt         = 0;
  self->cap         = cap;
  self->destroyFunc = destroyFunc;
  self->storage     = SDL_malloc(cap * sizeof(pointer_t));
  self->rear        = -1;
  self->front       = -1;
  return self;
}

Queue* queue_init(Queue* self, u32 cap)
{
  return queue_init_full(self, cap, NULL);
}

void queue_fini(Queue* self)
{
  if (self->destroyFunc != NULL)
    for (u32 i = 0; i < self->cap; ++i)
      self->destroyFunc(self->storage[i]);
  SDL_free(self->storage);
}

void queue_offer(Queue* self, pointer_t item)
{
  if (queue_empty(self))
  {
    self->front = self->rear   = 0;
    self->storage[self->front] = item;
    self->cnt++;
  }
  else
  {
    ASSERT(!queue_full(self) && "do not enough empty space");
    self->rear                = (self->rear + 1) % self->cap;
    self->storage[self->rear] = item;
    self->cnt++;
  }
}

pointer_t queue_poll(Queue* self)
{
  ASSERT(!queue_empty(self) && "empty queue");

  pointer_t v = self->storage[self->front];
  if (self->cnt == 1)
  {
    self->front = self->rear = -1;
    self->cnt--;
    return v;
  }
  else
  {
    self->front = (self->front + 1) % self->cap;
    self->cnt--;
  }
  return v;
}

pointer_t queue_peek(Queue* self)
{
  ASSERT(!queue_empty(self) && "empty queue");
  return self->storage[self->front];
}

void queue_clear(Queue* self)
{
  self->cnt  = 0;
  self->rear = self->front = -1;
}
