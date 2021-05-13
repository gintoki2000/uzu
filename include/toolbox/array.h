#ifndef ARRAY_H
#define ARRAY_H
#include "common.h"

#define ARRAY_DECL(T, Name, name)                                                                  \
  typedef struct Name                                                                              \
  {                                                                                                \
    T*  storage;                                                                                   \
    int count;                                                                                     \
    int capacity;                                                                                  \
  } Name;                                                                                          \
  Name* name##_init_w_cap(Name* self, int capacity);                                               \
  Name* name##_init(Name* self);                                                                   \
  void  name##_fini(Name* self);                                                                   \
  void  name##_add(Name* self, T val);                                                             \
  int   name##_index_of(Name* self, T val);                                                        \
  void  name##_rmv_index(Name* self, int index);                                                   \
  BOOL  name##_rmv_value(Name* self, T value);                                                     \
  void  name##_qrmv(Name* self, int index);                                                        \
  void  name##_expand(Name* self, int newCapacity);

#define ARRAY_IMPL(T, Name, name, cmp)                                                             \
  Name* name##_init_w_cap(Name* self, int capacity)                                                \
  {                                                                                                \
    self->count    = 0;                                                                            \
    self->capacity = capacity;                                                                     \
    self->storage  = SDL_calloc(capacity, sizeof(T));                                              \
    return self;                                                                                   \
  }                                                                                                \
  Name* name##_init(Name* self)                                                                    \
  {                                                                                                \
    return name##_init_w_cap(self, 16);                                                            \
  }                                                                                                \
  void name##_fini(Name* self)                                                                     \
  {                                                                                                \
    if (self)                                                                                      \
    {                                                                                              \
      SDL_free(self->storage);                                                                     \
      self->count    = 0;                                                                          \
      self->capacity = 0;                                                                          \
    }                                                                                              \
  }                                                                                                \
  void name##_add(Name* self, T val)                                                               \
  {                                                                                                \
    if (self->count == self->capacity)                                                             \
      name##_expand(self, self->capacity * 2);                                                     \
    self->storage[self->count++] = val;                                                            \
  }                                                                                                \
  int name##_index_of(Name* self, T val)                                                           \
  {                                                                                                \
    int count   = self->count;                                                                     \
    T*  storage = self->storage;                                                                   \
    for (int i = 0; i < count; ++i)                                                                \
      if (!cmp(storage[i], val))                                                                   \
        return i;                                                                                  \
    return -1;                                                                                     \
  }                                                                                                \
  void name##_rmv_index(Name* self, int index)                                                     \
  {                                                                                                \
    SDL_assert(index < self->count);                                                               \
    for (int i = 0; i < self->count - 1; ++i)                                                      \
    {                                                                                              \
      self->storage[i] = self->storage[i + 1];                                                     \
    }                                                                                              \
    --self->count;                                                                                 \
  }                                                                                                \
  BOOL name##_rmv_value(Name* self, T val)                                                         \
  {                                                                                                \
    int index = name##_index_of(self, val);                                                        \
    if (index != -1)                                                                               \
    {                                                                                              \
      name##_rmv_index(self, index);                                                               \
      return TRUE;                                                                                 \
    }                                                                                              \
    return FALSE;                                                                                  \
  }                                                                                                \
  void name##_qrmv(Name* self, int index)                                                          \
  {                                                                                                \
    ASSERT(index < self->count);                                                                   \
    if (self->count == 0)                                                                          \
    {                                                                                              \
      --self->count;                                                                               \
      return;                                                                                      \
    }                                                                                              \
    self->storage[index] = self->storage[self->count - 1];                                         \
    --self->count;                                                                                 \
  }                                                                                                \
  void name##_expand(Name* self, int newCapacity)                                                  \
  {                                                                                                \
    if (self->capacity >= newCapacity)                                                             \
      return;                                                                                      \
    self->storage  = SDL_realloc(self->storage, sizeof(T) * newCapacity);                          \
    self->capacity = newCapacity;                                                                  \
  }

#endif // ARRAY_H
