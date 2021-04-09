#ifndef COMMON_H
#define COMMON_H
#include "SDL.h"
#include "SDL_FontCache.h"
#include <stdbool.h>

typedef Sint8  s8;
typedef Uint8  u8;
typedef Sint16 s16;
typedef Uint16 u16;
typedef Sint32 s32;
typedef Uint32 u32;

typedef const void* constpointer_t;
typedef void*       pointer_t;
typedef int (*compare_fn_t)(constpointer_t lhs, constpointer_t rhs);
typedef void (*destroy_fn_t)(pointer_t p);
typedef bool (*equal_fn_t)(constpointer_t lhs, constpointer_t rhs);
typedef bool (*consume_fn_t)(pointer_t user_data, pointer_t val);
typedef void (*funcptr_t)();

#define RECT SDL_Rect
#define POINT SDL_Point
#define COLOR SDL_Color
#define POINT SDL_Point
#define FONT FC_Font
#define RENDERER SDL_Renderer
#define TEXTURE SDL_Texture
#define FLIP SDL_RendererFlip
#define TEXT_EFFECT FC_TextEffect

typedef struct
{
  pointer_t user_data;
  funcptr_t func;
} Callback;

#define BOOL SDL_bool
#define TRUE SDL_TRUE
#define FALSE SDL_FALSE

#define TIMER_TICK(__timer) ((__timer) > 0 && --(__timer) == 0)

#define UNUSED(__var) (void)__var

#define FLIP_TO_SIGN(__f) (__f == SDL_FLIP_NONE ? 1 : -1)

#define ASSERT(cd) SDL_assert(cd)

#define CALLBACK_1(_user_data, _func)                                                              \
  ((Callback){                                                                                     \
      .user_data = (pointer_t)_user_data,                                                          \
      .func      = (funcptr_t)_func,                                                               \
  })
#define CALLBACK_2(__func) ((Callback){ .func = (funcptr_t)__func })

#define INVOKE_CALLBACK(__callback, __return_type, ...)                                            \
  (((__return_type(*)())(__callback).func)((__callback).user_data, __VA_ARGS__))

#define INVOKE_CALLBACK_NOARGS(__callback, __return_type)                                          \
  (((__return_type(*)())(__callback).func)((__callback).user_data))

#define INVOKE_EVENT(__callback, ...)                                                              \
  do                                                                                               \
  {                                                                                                \
    if ((__callback).func != NULL)                                                                 \
      (((void (*)())(__callback).func)((__callback).user_data, __VA_ARGS__));                      \
  } while (0)

#define INLINE static inline

#define DEG_TO_RAD 0.0174532925

#define MAX(__a, __b)                                                                              \
  ({                                                                                               \
    __auto_type __tmp_a = (__a);                                                                   \
    __auto_type __tmp_b = (__b);                                                                   \
    __tmp_a > __tmp_b ? __tmp_a : __tmp_b;                                                         \
  })

#define MIN(__a, __b)                                                                              \
  ({                                                                                               \
    __auto_type __tmp_a = (__a);                                                                   \
    __auto_type __tmp_b = (__b);                                                                   \
    __tmp_a < __tmp_b ? __tmp_a : __tmp_b;                                                         \
  })

#define BIT(__x) (1 << (__x))

#define ABS(__x)                                                                                   \
  ({                                                                                               \
    __auto_type __tmp = __x;                                                                       \
    __tmp > 0 ? __tmp : -__tmp;                                                                    \
  })

#define SWAP(__a, __b)                                                                             \
  ({                                                                                               \
    __auto_type __tmp = (__a);                                                                     \
    __a               = __b;                                                                       \
    __b               = __tmp;                                                                     \
  })

#define EPSILON 0.001f

#define STREQ(a, b) (SDL_strcmp(a, b) == 0)

typedef struct
{
  float x;
  float y;
} Vec2;

typedef struct
{
  float x;
  float y;
  float z;
} Vec3;

typedef struct
{
  int x, y;
} Vec2i;

#define VEC2(_x, _y) ((Vec2){ .x = (_x), .y = (_y) })
#define VEC2_ZERO VEC2(0.f, 0.f)
#define VEC2_UP VEC2(0.f, -1.f)
#define VEC2_DOWN VEC2(0.f, 1.f)
#define VEC2_LEFT VEC2(-1.f, 0.f)
#define VEC2_RIGHT VEC2(1.f, 0.f)

typedef struct
{
  Vec2 lower_bound;
  Vec2 upper_bound;
} AABB;

INLINE int max(int a, int b)
{
  return a > b ? a : b;
}

INLINE int min(int a, int b)
{
  return a < b ? a : b;
}

INLINE float maxf(float a, float b)
{
  return a > b ? a : b;
}

INLINE float minf(float a, float b)
{
  return a < b ? a : b;
}

INLINE float signf(float x)
{ return (x > 0) - (x < 0); }

INLINE int sign(int x)
{ return (x > 0) - (x < 0); }

INLINE Vec2 vec2_max(Vec2 a, Vec2 b)
{
  return (Vec2){ maxf(a.x, b.x), maxf(a.y, b.y) };
}

INLINE Vec2 vec2_min(Vec2 a, Vec2 b)
{
  return (Vec2){ minf(a.x, b.x), minf(a.y, b.y) };
}

INLINE float absf(float x)
{
  return x >= 0.f ? x : -x;
}

INLINE Vec2 vec2_sub(Vec2 a, Vec2 b)
{
  return (Vec2){ a.x - b.x, a.y - b.y };
}

INLINE Vec2 vec2_mul(Vec2 a, float k)
{
  return (Vec2){ a.x * k, a.y * k };
}

INLINE float vec2_mag(Vec2 v)
{
  return SDL_sqrt(v.x * v.x + v.y * v.y);
}

INLINE float vec2_normalize(Vec2* v)
{
  float mag = vec2_mag(*v);
  if (mag < EPSILON)
  {
    return 0.f;
  }
  float ivmag = 1.f / mag;
  v->x *= ivmag;
  v->y *= ivmag;
  return mag;
}

INLINE float invsqrt(float x)
{
  float xhalf = 0.5f * x;
  int   i     = *(int*)&x;                  // store floating-point bits in integer
  i           = 0x5f3759df - (i >> 1);      // initial guess for Newton's method
  x           = *(float*)&i;                // convert new bits into float
  x           = x * (1.5f - xhalf * x * x); // One round of Newton's method
  return x;
}

INLINE void vec2_scale_to_length(Vec2* v, float len)
{
  float ivlen = invsqrt(v->x * v->x + v->y * v->y);
  v->x *= ivlen;
  v->y *= ivlen;
  v->x *= len;
  v->y *= len;
}

INLINE float vec2_mag2(Vec2 v)
{
  return v.x * v.x + v.y * v.y;
}

INLINE Vec2 vec2_unit(Vec2 v)
{
  float len2  = vec2_mag2(v);
  float ivlen = invsqrt(len2);
  return VEC2(v.x * ivlen, v.y * ivlen);
}

INLINE Vec2 vec2_trunc(Vec2 v, float max_len)
{
  if (vec2_mag2(v) > max_len * max_len)
  {
    return vec2_mul(vec2_unit(v), max_len);
  }
  return v;
}

INLINE float vec2_dist(Vec2 a, Vec2 b)
{
  return vec2_mag(vec2_sub(a, b));
}

INLINE bool aabb_test_overlap(const AABB* a, const AABB* b)
{
  float dx1 = b->lower_bound.x - a->upper_bound.x;
  float dy1 = b->lower_bound.y - a->upper_bound.y;
  float dx2 = a->lower_bound.x - b->upper_bound.x;
  float dy2 = a->lower_bound.y - b->upper_bound.y;
  if (dx1 > 0.f || dy1 > 0.f)
    return false;
  if (dx2 > 0.f || dy2 > 0.f)
    return false;
  return true;
}

INLINE AABB aabb_merge(const AABB* a, const AABB* b)
{
  return (AABB){
    .lower_bound = vec2_min(a->lower_bound, b->lower_bound),
    .upper_bound = vec2_max(a->upper_bound, b->upper_bound),
  };
}

INLINE float aabb_premiter(const AABB* aabb)
{
  float w = aabb->upper_bound.x - aabb->lower_bound.x;
  float h = aabb->upper_bound.y - aabb->lower_bound.y;
  return 2.f * (w + h);
}

INLINE void aabb_extend(AABB* aabb, float v)
{
  aabb->lower_bound.x -= v;
  aabb->lower_bound.y -= v;
  aabb->upper_bound.x += v;
  aabb->upper_bound.y += v;
}

INLINE bool aabb_contains(const AABB* a, const AABB* b)
{
  bool result = true;
  result      = result && a->lower_bound.x <= b->lower_bound.x;
  result      = result && a->lower_bound.y <= b->lower_bound.y;
  result      = result && b->upper_bound.x <= a->upper_bound.x;
  result      = result && b->upper_bound.y <= a->upper_bound.y;
  return result;
}

#endif // COMMON_H
