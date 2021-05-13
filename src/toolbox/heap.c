#include "toolbox/heap.h"
#include <stdbool.h>
#include <stdlib.h>

#define HEAP_DEFAULT_CAPACITY 16

INLINE void ensure_capacity(Heap* heap)
{
  if (heap->cnt == heap->cap)
  {
    heap->cap *= 2;
    heap->storage = SDL_realloc(heap->storage, heap->cap * sizeof(pointer_t));
  }
}
INLINE int  left_child_index(int parent_index) 
{ return 2 * parent_index + 1; }

INLINE int  right_child_index(int parent_index) 
{ return 2 * parent_index + 2; }

INLINE int  parent_index(int child_index) 
{ return (child_index - 1) / 2; }

INLINE BOOL has_left_child(Heap* heap, int index)
{ return left_child_index(index) < heap->cnt; }

INLINE BOOL has_right_child(Heap* heap, int index)
{ return right_child_index(index) < heap->cnt; }

INLINE BOOL has_parent(int index) 
{ return parent_index(index) >= 0; }

INLINE pointer_t left_child(Heap* heap, int index)
{ return heap->storage[left_child_index(index)]; }

INLINE pointer_t right_child(Heap* heap, int index)
{ return heap->storage[right_child_index(index)]; }

INLINE pointer_t parent(Heap* heap, int index) 
{ return heap->storage[parent_index(index)]; }

INLINE void swap(Heap* heap, int lhs, int rhs)
{
  pointer_t tmp      = heap->storage[lhs];
  heap->storage[lhs] = heap->storage[rhs];
  heap->storage[rhs] = tmp;
}

INLINE void heapify_up(Heap* heap)
{
  int index = heap->cnt - 1;
  int parent_idx;

  while (has_parent(index) && heap->compareFunc(parent(heap, index), heap->storage[index]) > 0)
  {
    parent_idx = parent_index(index);
    swap(heap, parent_idx, index);
    index = parent_idx;
  }
}

INLINE void heapify_down(Heap* heap)
{
  int          index      = 0;
  pointer_t*   storage    = heap->storage;
  CompareFunc compareFunc = heap->compareFunc;
  while (has_left_child(heap, index))
  {
    int smaller_child_index = left_child_index(index);
    if (has_right_child(heap, index) &&
        compareFunc(right_child(heap, index), left_child(heap, index)) < 0)
    {
      smaller_child_index = right_child_index(index);
    }

    if (compareFunc(storage[index], storage[smaller_child_index]) < 0)
    {
      break;
    }
    else
    {
      swap(heap, index, smaller_child_index);
      index = smaller_child_index;
    }
  }
}

static Heap* heap_alloc()
{ return SDL_malloc(sizeof(Heap)); }

Heap* heap_new(CompareFunc compareFunc, DestroyFunc destroyFunc)
{
  return heap_init(heap_alloc(), compareFunc, destroyFunc);
}

void heap_delete(Heap* heap)
{
  if (heap)
  {
    heap_fini(heap);
    SDL_free(heap);
  }
}

Heap* heap_init(Heap* self, CompareFunc compareFunc, DestroyFunc destroyFunc)
{
  self->compareFunc = compareFunc;
  self->destroyFunc = destroyFunc;
  self->cnt        = 0;
  self->cap        = HEAP_DEFAULT_CAPACITY;
  self->storage    = SDL_malloc(sizeof(pointer_t) * HEAP_DEFAULT_CAPACITY);
  return self;
}

void heap_fini(Heap* self)
{
  if (self->destroyFunc != NULL)
  {
    for (int i = 0; i < self->cnt; ++i)
    {
      self->destroyFunc(self->storage[i]);
    }
  }
  SDL_free(self->storage);
}

pointer_t heap_peak(Heap* heap)
{
  ASSERT(heap->cnt > 0 && "heap empty");
  return heap->storage[0];
}

pointer_t heap_poll(Heap* heap)
{
  ASSERT(heap->cnt > 0 && "heap empty");
  pointer_t item   = heap->storage[0];
  heap->storage[0] = heap->storage[heap->cnt - 1];
  heap->cnt--;
  heapify_down(heap);
  return item;
}

void heap_add(Heap* heap, pointer_t v)
{
  ensure_capacity(heap);
  heap->storage[heap->cnt] = v;
  heap->cnt++;
  heapify_up(heap);
}
