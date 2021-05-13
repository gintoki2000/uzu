#include "toolbox/ptrarray.h"
#include <assert.h>
#include <stdlib.h>

INLINE PtrArray* ptr_array_alloc()
{
  return (PtrArray*)malloc(sizeof(PtrArray));
}

PtrArray* ptr_array_new(DestroyFunc destroyFunc)
{
  return ptr_array_init(ptr_array_alloc(), destroyFunc);
}

PtrArray* ptr_array_new_copy(const PtrArray* other)
{
  return ptr_array_init_copy(ptr_array_alloc(), other);
}

void ptr_array_delete(PtrArray* arr)
{
  if (arr)
  {
    ptr_array_fini(arr);
    free(arr);
  }
}

PtrArray* ptr_array_init(PtrArray* self, DestroyFunc destroyFunc)
{
  return ptr_array_init_w_cap(self, destroyFunc, 16);
}

PtrArray* ptr_array_init_w_cap(PtrArray* self, DestroyFunc destroyFunc, s32 cap)
{
  self->cap         = cap;
  self->cnt         = 0;
  self->destroyFunc = destroyFunc;
  self->storage     = SDL_malloc(sizeof(pointer_t) * self->cap);
  return self;
}

PtrArray* ptr_array_init_copy(PtrArray* self, const PtrArray* other)
{
  ptr_array_init_w_cap(self, NULL, other->cnt);
  SDL_memcpy(self->storage, other->storage, other->cnt * sizeof(void*));
  self->cnt = other->cnt;
  return self;
}

void ptr_array_fini(PtrArray* self)
{
  if (self->destroyFunc != NULL)
  {
    for (s32 i = 0; i < self->cnt; ++i)
    {
      self->destroyFunc(self->storage[i]);
    }
  }
  free(self->storage);
}

pointer_t ptr_array_add(PtrArray* self, pointer_t p)
{
  if (self->cnt == self->cap)
  {
    ptr_array_reserve(self, self->cap * 2);
  }

  return self->storage[self->cnt++] = p;
}

BOOL ptr_array_rmv(PtrArray* self, pointer_t p)
{
  int idx = ptr_array_index_of(self, p);
  if (idx != -1)
  {
    ptr_array_rmv_idx(self, idx);
    return true;
  }
  return false;
}

void ptr_array_rmv_idx(PtrArray* self, s32 idx)
{
  assert(idx >= 0 && idx < self->cnt && "out of index");
  int        cnt = self->cnt;
  pointer_t* a   = self->storage;
  for (int i = idx; i < cnt - 1; ++i)
  {
    a[i] = a[i + 1];
  }
  --self->cnt;
}

void ptr_array_qrmv(PtrArray* self, s32 idx)
{
  assert(idx >= 0 && idx < self->cnt && "out of index");
  if (self->destroyFunc != NULL)
  {
    self->destroyFunc(self->storage[idx]);
  }

  self->storage[idx] = self->storage[self->cnt - 1];
  self->cnt--;
}

int ptr_array_index_of(PtrArray* self, pointer_t p)
{
  int        cnt = self->cnt;
  pointer_t* a   = self->storage;
  for (int i = 0; i < cnt; ++i)
    if (a[i] == p)
      return i;
  return -1;
}

BOOL ptr_array_contains(PtrArray* self, pointer_t p)
{
  return ptr_array_index_of(self, p) != -1;
}

void ptr_array_reserve(PtrArray* self, s32 n)
{
  if (n <= self->cap)
    return;
  self->cap     = n;
  self->storage = SDL_realloc(self->storage, self->cap * sizeof(pointer_t));
}

void ptr_array_sort(PtrArray* self, __compar_fn_t comp)
{
  SDL_qsort(self->storage, self->cnt, sizeof(void*), comp);
}

void ptr_array_clear(PtrArray* self)
{
  if (self->destroyFunc != NULL)
  {
    for (s32 i = 0; i < self->cnt; ++i)
    {
      self->destroyFunc(self->storage[i]);
    }
  }
  self->cnt = 0;
}
