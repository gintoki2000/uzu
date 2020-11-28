#ifndef ARRAY_H
#define ARRAY_H
#include "types.h"
typedef struct Array
{
  s8* storage;
  u32 cnt;
  u32 cap;
  u32 element_size;
} Array;

Array* array_new();
void   array_delete();

Array* array_init(Array* self);
void   array_fini(Array* self);

void* _array_addn(Array* self, u32 n);

/**/
#define array_at(self, idx, T) ((T*) _array_at(self, idx))
void* _array_at(Array* self, u32 idx);
#endif // ARRAY_H
