#ifndef PTRARRAY_H
#define PTRARRAY_H
#include "common.h"
#include <stdlib.h>
typedef struct PtrArray
{
  pointer_t*  storage;
  s32         cnt;
  s32         cap;
  DestroyFunc destroyFunc;
} PtrArray;

PtrArray* ptr_array_new(DestroyFunc destroyFunc);
PtrArray* ptr_array_new_copy(const PtrArray* other);
void      ptr_array_delete(PtrArray* arr);

PtrArray* ptr_array_init(PtrArray* self, DestroyFunc destroyFunc);
PtrArray* ptr_array_init_w_cap(PtrArray* self, DestroyFunc destroyFunc, s32 cap);
PtrArray* ptr_array_init_copy(PtrArray* self, const PtrArray* other);
PtrArray* ptr_array_init_w_array(PtrArray* self, DestroyFunc destroyFunc, pointer_t* data, s32 n);
void      ptr_array_fini(PtrArray* self);

pointer_t ptr_array_add(PtrArray* self, pointer_t p);
BOOL      ptr_array_rmv(PtrArray* self, pointer_t p);
void      ptr_array_rmv_idx(PtrArray* self, s32 idx);
void      ptr_array_qrmv(PtrArray* self, s32 idx);

int  ptr_array_index_of(PtrArray* self, pointer_t p);
BOOL ptr_array_contains(PtrArray* self, pointer_t p);
void ptr_array_reserve(PtrArray* self, s32 n);
void ptr_array_sort(PtrArray* self, CompareFunc comp);
void ptr_array_clear(PtrArray* self);

#define ptr_array_storage(__T, __self) ((__T**)(__self)->storage)
#define ptr_array_at(__self, idx) ((__self)->storage[(idx)])

#endif // PTRARRAY_H
