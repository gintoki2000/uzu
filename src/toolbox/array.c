#include <stdlib.h>
#include <string.h>
#include "toolbox/array.h"
#define ARRAY_DEFAULT_CAP 16

Array* _array_new(u32 el_size) { return array_init(malloc(sizeof(Array)), el_size); }

void array_del(Array* arr)
{
  if (arr != NULL)
  {
    array_fini(arr);
    free(arr);
  }
}

Array* array_init(Array* self, u32 el_size)
{
  self->cnt = 0;
  self->cap = ARRAY_DEFAULT_CAP;
  self->el_size = el_size;
  self->storage = malloc(sizeof(s8) * ARRAY_DEFAULT_CAP);
  return self;
}

void array_fini(Array *_self)
{
  free(_self->storage);
}

void* _array_addn(Array* self, u32 n)
{
  u32 ncap;

  if (self->cnt == self->cap) {
  	ncap = self->cap;
  	while (ncap < n)
    	ncap *= 2;
        array_reserve(self, ncap);
  }
  s8* ret = _array_at(self, self->cnt);
  self->cnt += n;
  return ret;
}

void* _array_at(Array *self, u32 idx)
{
	return self->storage + (idx * self->el_size);
}

u32 array_rmv(Array *self, u32 idx)
{
	for (u32 i = idx; i < self->cnt - 1; ++i)
		memcpy(_array_at(self, i), _array_at(self, i + 1), self->el_size);
	return self->cnt--;
}

u32 array_qrmv(Array *self, u32 idx)
{
	if (idx != self->cnt - 1)
		memcpy(_array_at(self, idx), _array_at(self, self->cnt - 1), self->el_size);	
	return self->cnt--;
}

void array_reserve(Array* self, u32 new_cap)
{
	if (new_cap <= self->cap)
		return;
	self->storage = realloc(self->storage, sizeof(s8) * new_cap);
	self->cap = new_cap;
}
