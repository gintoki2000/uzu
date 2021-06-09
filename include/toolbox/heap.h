#ifndef HEAP_H
#define HEAP_H
#include "common.h"

typedef struct Heap
{
  int         size;
  int         count;
  pointer_t*  storage;
  DestroyFunc destroyFunc;
  CompareFunc compareFunc;
} Heap;

Heap* heap_create(CompareFunc compareFunc, DestroyFunc destroyFunc);
void  heap_free(Heap* heap);

Heap*     heap_init(Heap* self, CompareFunc, DestroyFunc destroyFunc);
void      heap_fini(Heap* self);
pointer_t heap_peak(Heap* heap);
pointer_t heap_poll(Heap* heap);
void      heap_add(Heap* heap, pointer_t v);

INLINE BOOL heap_empty(const Heap* heap)
{ return heap->count == 0; }

#endif // HEAP_H
