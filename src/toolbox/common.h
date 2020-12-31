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
  void (*func)(void);
} Callback;

#define BOOL SDL_bool
#define TRUE SDL_TRUE
#define FALSE SDL_FALSE
#define RECT SDL_Rect
#define POINT SDL_Point
#define COLOR SDL_Color
#define POINT SDL_Point
#define FONT FC_Font

#define FLIP_TO_SIGN(__f) (__f == SDL_FLIP_NONE ? 1 : -1)

#define ASSERT(cd) SDL_assert(cd)

#define CALLBACK_1(_user_data, _func)                                                              \
  ((Callback){                                                                                     \
      .user_data = (void*)_user_data,                                                              \
      .func      = (void (*)(void))_func,                                                          \
  })
#define CALLBACK_2(_func) ((Callback){ .func = (void (*)(void))_func })
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
    typeof(__a) __tmp = (__a);                                                                     \
    __a               = __b;                                                                       \
    __b               = __tmp;                                                                     \
  })

#define SIN SDL_sin
#define COS SDL_cos
#define SQRTF SDL_sqrtf

#define EPSILON 0.001f

typedef struct
{
  float x;
  float y;
} Vec2;

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
  return SQRTF(v.x * v.x + v.y * v.y);
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

INLINE void vec2_scale_to_length(Vec2* v, float len)
{
  vec2_normalize(v);
  v->x *= len;
  v->y *= len;
}

INLINE Vec2 vec2_unit_vec(Vec2 v)
{
  float len   = vec2_mag(v);
  float ivlen = 1.f / len;
  return VEC2(v.x * ivlen, v.y * ivlen);
}

INLINE Vec2 vec2_trunc(Vec2 v, float max_len)
{
  if (vec2_mag(v) > max_len)
  {
    return vec2_mul(vec2_unit_vec(v), max_len);
  }
  return v;
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
