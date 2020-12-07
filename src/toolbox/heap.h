#ifndef HEAP_H
#define HEAP_H
#include "common.h"
#define HEAP(__obj) ((Heap*)__obj)

typedef struct Heap
{
  u32          cap;
  u32          cnt;
  pointer_t*   storage;
  destroy_fn_t destroy_fn;
  compare_fn_t compare_fn;
} Heap;

Heap* heap_alloc();
Heap* heap_new(compare_fn_t compare_fn, destroy_fn_t destroy_fn);
void  heap_delete(Heap* heap);

Heap* heap_init(Heap* self, compare_fn_t, destroy_fn_t destroy_fn);
void  heap_fini(Heap* self);

pointer_t heap_peak(Heap* heap);
pointer_t heap_poll(Heap* heap);
void      heap_add(Heap* heap, pointer_t v);

#endif // HEAP_H
