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

#define array_new(_T) (_array_new(sizeof(_T)))
Array* _array_new(u32 el_size);
void   array_del(Array* arr);

Array* array_init(Array* _self, u32 el_size);
void   array_fini(Array* _self);

void* _array_addn(Array* _self, u32 n);
#define array_add(_T, _self, _el) ((*(_T*)_array_addn(_self, 1)) = _el)

u32 array_rmv(Array* _self, u32 idx);
u32 array_qrmv(Array* _self, u32 idx);

#define array_at(_T, _self, _idx) ((_T*)_array_at(_self, _idx))
void* _array_at(Array* _self, u32 idx);

void array_encap(Array* _self, u32 new_cap);
#endif // ARRAY_H
