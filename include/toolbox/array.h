#ifndef ARRAY_H
#define ARRAY_H
#include "common.h"
typedef struct Array
{
  s8* storage;
  u32 cnt;
  u32 cap;
  u32 el_size;
} Array;

#define array_new(T) (_array_new(sizeof(T)))
Array* _array_new(u32 el_size);
void   array_del(Array* arr);

Array* array_init(Array* self, u32 el_size);
void   array_fini(Array* self);

void* _array_addn(Array* self, u32 n);
#define array_add(T, self, el) ((*(T*)_array_addn(self, 1)) = el)

u32 array_rmv(Array* self, u32 idx);
u32 array_qrmv(Array* self, u32 idx);

#define array_at(T, self, idx) ((T*)_array_at(self, idx))
void* _array_at(Array* self, u32 idx);

void array_reserve(Array* self, u32 new_cap);
#endif // ARRAY_H
