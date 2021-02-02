#include "toolbox/heap.h"
#include <stdbool.h>
#include <stdlib.h>

#define HEAP_DEFAULT_CAPACITY 16

INLINE void ensure_capacity(Heap* heap)
{
  if (heap->cnt == heap->cap)
  {
    heap->cap *= 2;
    heap->storage = realloc(heap->storage, heap->cap * sizeof(pointer_t));
  }
}
INLINE int  get_left_child_index(int parent_index) { return 2 * parent_index + 1; }
INLINE int  get_right_child_index(int parent_index) { return 2 * parent_index + 2; }
INLINE int  get_parent_index(int child_index) { return (child_index - 1) / 2; }
INLINE BOOL has_left_child(Heap* heap, int index)
{
  return get_left_child_index(index) < heap->cnt;
}
INLINE BOOL has_right_child(Heap* heap, int index)
{
  return get_right_child_index(index) < heap->cnt;
}
INLINE BOOL has_parent(int index) { return get_parent_index(index) >= 0; }

INLINE pointer_t left_child(Heap* heap, int index)
{
  return heap->storage[get_left_child_index(index)];
}

INLINE pointer_t right_child(Heap* heap, int index)
{
  return heap->storage[get_right_child_index(index)];
}

INLINE pointer_t parent(Heap* heap, int index) { return heap->storage[get_parent_index(index)]; }

INLINE void swap(Heap* heap, int lhs, int rhs)
{
  pointer_t tmp      = heap->storage[lhs];
  heap->storage[lhs] = heap->storage[rhs];
  heap->storage[rhs] = tmp;
}

INLINE void heapify_up(Heap* heap)
{
  int index = heap->cnt - 1;
  int parent_index;

  while (has_parent(index) && heap->compare_fn(parent(heap, index), heap->storage[index]) > 0)
  {
    parent_index = get_parent_index(index);
    swap(heap, parent_index, index);
    index = parent_index;
  }
}

INLINE void heapify_down(Heap* heap)
{
  int          index      = 0;
  pointer_t*   storage    = heap->storage;
  compare_fn_t compare_fn = heap->compare_fn;
  while (has_left_child(heap, index))
  {
    int smaller_child_index = get_left_child_index(index);
    if (has_right_child(heap, index) &&
        compare_fn(right_child(heap, index), left_child(heap, index)) < 0)
    {
      smaller_child_index = get_right_child_index(index);
    }

    if (compare_fn(storage[index], storage[smaller_child_index]) < 0)
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

Heap* heap_alloc() { return (Heap*)malloc(sizeof(Heap)); }

Heap* heap_new(compare_fn_t compare_fn, destroy_fn_t destroy_fn)
{
  return heap_init(heap_alloc(), compare_fn, destroy_fn);
}

void heap_delete(Heap* heap)
{
  if (heap)
  {
    heap_fini(heap);
    free(heap);
  }
}

Heap* heap_init(Heap* self, compare_fn_t compare_fn, destroy_fn_t destroy_fn)
{
  self->compare_fn = compare_fn;
  self->destroy_fn = destroy_fn;
  self->cnt        = 0;
  self->cap        = HEAP_DEFAULT_CAPACITY;
  self->storage    = malloc(sizeof(pointer_t) * HEAP_DEFAULT_CAPACITY);
  return self;
}

void heap_fini(Heap* self)
{
  if (self->destroy_fn != NULL)
  {
    for (u32 i = 0; i < self->cnt; ++i)
    {
      self->destroy_fn(self->storage[i]);
    }
  }
  free(self->storage);
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
