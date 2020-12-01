#include "rect.h"

INLINE Vec2 rotate(Vec2 p, Vec2 O, double angle)
{
  double a = angle * DEG_TO_RAD;
  double c = SDL_cos(a);
  double s = SDL_sin(a);

  return (Vec2){
    c * (p.x - O.x) - s * (p.y - O.y) + O.x,
    s * (p.x - O.x) + c * (p.y - O.y) + O.y,
  };
}

Rect* rect_init_full(Rect* rect, float x, float y, float w, float h, float cx, float cy, double angle)
{
  Vec2 O;

  O.x = x + cx;
  O.y = y + cy;

  rect->v1 = rotate(VEC2(x, y), O, angle);
  rect->v2 = rotate(VEC2(x + w, y), O, angle);
  rect->v3 = rotate(VEC2(x + w, y + h), O, angle);
  rect->v4 = rotate(VEC2(x, y + h), O, angle);
  return rect;
}

INLINE float rect_min_x(const Rect* r)
{
  float minx;

  minx = minf(r->v1.x, r->v2.x);
  minx = minf(minx, r->v3.x);
  minx = minf(minx, r->v4.x);
  return minx;
}

INLINE float rect_min_y(const Rect* r)
{
  float miny;

  miny = minf(r->v1.y, r->v2.y);
  miny = minf(miny, r->v3.y);
  miny = minf(miny, r->v4.y);
  return miny;
}

INLINE float rect_max_x(const Rect* r)
{
  float maxx;
  maxx = maxf(r->v1.x, r->v2.x);
  maxx = maxf(maxx, r->v3.x);
  maxx = maxf(maxx, r->v4.x);
  return maxx;
}

INLINE float rect_max_y(const Rect* r)
{
  float maxy;
  maxy = maxf(r->v1.y, r->v2.y);
  maxy = maxf(maxy, r->v3.y);
  maxy = maxf(maxy, r->v4.y);
  return maxy;
}

BOOL rect_has_intersection(const Rect* r1, const Rect* r2)
{
  float min_x1, min_y1;
  float max_x1, max_y1;
  float min_x2, min_y2;
  float max_x2, max_y2;

  min_x1 = rect_min_x(r1);
  max_x1 = rect_max_x(r1);

  min_y1 = rect_min_y(r1);
  max_y1 = rect_max_y(r1);

  min_x2 = rect_min_x(r2);
  max_x2 = rect_max_x(r2);

  min_y2 = rect_min_y(r2);
  max_y2 = rect_max_y(r2);

  BOOL overlap_x = (min_x2 >= min_x1 && min_x2 <= max_x1) || (min_x1 >= min_x2 && min_x1 <= max_x2);
  BOOL overlap_y = (min_y2 >= min_y1 && min_y2 <= max_y1) || (min_y1 >= min_y2 && min_y1 <= max_y2);

  return overlap_x && overlap_y;
}
