#ifndef PTRARRAY_H
#define PTRARRAY_H
#include "common.h"
#include <stdlib.h>
typedef struct PtrArray
{
  pointer_t*  storage;
  s32         count;
  s32         size;
  DestroyFunc destroyFunc;
} PointerArray;

PointerArray* pointer_array_create(DestroyFunc destroyFunc);
PointerArray* pointer_array_create_sized(DestroyFunc destroyFunc, int size);
void          pointer_array_free(PointerArray* arr);

PointerArray* pointer_array_init(PointerArray* self, DestroyFunc destroyFunc);
PointerArray* pointer_array_init_sized(PointerArray* self, DestroyFunc destroyFunc, s32 size);
void pointer_array_fini(PointerArray* self);

pointer_t pointer_array_add(PointerArray* self, pointer_t p);
BOOL      pointer_array_rmv(PointerArray* self, pointer_t p);
void      pointer_array_rmv_idx(PointerArray* self, s32 idx);
void      pointer_array_qrmv(PointerArray* self, s32 idx);

int  pointer_array_find(PointerArray* self, pointer_t p);
BOOL pointer_array_contains(PointerArray* self, pointer_t p);
void pointer_array_reserve(PointerArray* self, s32 n);
void pointer_array_sort(PointerArray* self, CompareFunc comp);
void pointer_array_clear(PointerArray* self);

#define ptr_array_storage(__T, __self) ((__T**)(__self)->storage)
#define ptr_array_at(__self, idx) ((__self)->storage[(idx)])

#endif // PTRARRAY_H
