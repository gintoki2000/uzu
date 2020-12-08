#ifndef PTRARRAY_H
#define PTRARRAY_H
#include "common.h"
#include <stdlib.h>
typedef struct PtrArray
{
  pointer_t*   storage;
  u32          cnt;
  u32          cap;
  destroy_fn_t destroy_fn;
} PtrArray;

PtrArray* ptr_array_new(destroy_fn_t destroy_fn);
PtrArray* ptr_array_new_copy(const PtrArray* other);
void      ptr_array_delete(PtrArray* arr);

PtrArray* ptr_array_init(PtrArray* self, destroy_fn_t destroy_fn);
PtrArray* ptr_array_init_w_cap(PtrArray* self, destroy_fn_t destroy_fn, u32 cap);
PtrArray* ptr_array_init_copy(PtrArray* self, const PtrArray* other);
PtrArray* ptr_array_init_w_array(PtrArray* self, destroy_fn_t destroy_fn, pointer_t* data, u32 n);
void      ptr_array_fini(PtrArray* self);

pointer_t ptr_array_add(PtrArray* self, pointer_t p);
BOOL      ptr_array_remove(PtrArray* self, pointer_t p);
void      ptr_array_remove_idx(PtrArray* self, u32 idx);
void      ptr_array_swap_and_pop(PtrArray* self, u32 idx);

int  ptr_array_index_of(PtrArray* self, pointer_t p);
BOOL ptr_array_contains(PtrArray* self, pointer_t p);

void ptr_array_reserve(PtrArray* self, u32 n);
void ptr_array_shink_to_fit(PtrArray* self);

void ptr_array_sort(PtrArray* self, __compar_fn_t comp);
void ptr_array_foreach(PtrArray* self, consume_fn_t consume_fn, pointer_t user_data);

#define ptr_array_storage(__T, __self) ((__T**)(__self)->storage)
#define ptr_array_at(__self, idx) ((__self)->storage[(idx)])

#endif // PTRARRAY_H
