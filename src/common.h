#ifndef COMMON_H
#define COMMON_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <stdbool.h>

typedef Sint8  s8;
typedef Uint8  u8;
typedef Sint16 s16;
typedef Uint16 u16;
typedef Sint32 s32;
typedef Uint32 u32;

typedef char* string;

typedef void* pointer_t;
typedef int (*compare_fn_t)(const pointer_t lhs, const pointer_t rhs);
typedef void (*destroy_fn_t)(pointer_t p);
typedef int (*hash_fn_t)(const pointer_t p);
typedef bool (*equal_fn_t)(const pointer_t lhs, const pointer_t rhs);
typedef bool (*consume_fn_t)(pointer_t user_data, pointer_t val);
typedef bool (*bicomsume_fn_t)(pointer_t user_data, pointer_t, pointer_t);

typedef struct
{
  pointer_t user_data;
  pointer_t func;
} Callback;

#define CALLBACK_1(user_data, func)                                                                \
  (Callback) { .user_data = user_data, .func = func }
#define CALLBACK_2(func)                                                                           \
  (Callback) { .func = func }
#define INLINE static inline

#endif //COMMON_H
