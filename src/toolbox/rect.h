#ifndef RECT_H
#define RECT_H
#include "common.h"
typedef struct Rect
{
  Vec2 v1;
  Vec2 v2;
  Vec2 v3;
  Vec2 v4;
} Rect;

Rect* rect_init_full(Rect*  rect,
                     float  x,
                     float  y,
                     float  w,
                     float  h,
                     float  cx,
                     float  cy,
                     double angle);

INLINE Rect* rect_init(Rect* rect, float x, float y, float w, float h, double angle)
{
  return rect_init_full(rect, x, y, w, h, w / 2.f, h / 2.f, angle);
}

BOOL rect_has_intersection(const Rect* r1, const Rect* r2);

AABB* rect_get_aabb(const Rect* rect, AABB* aabb);
#endif // RECT_H
