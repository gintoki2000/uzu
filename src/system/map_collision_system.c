#include "map_collision_system.h"
#include <components.h>
#include <constances.h>
#include <toolbox/toolbox.h>
#include <map.h>

typedef struct FloatRect
{
  float x, y, w, h;
} FloatRect;

#define FLOAT_RECT(_x, _y, _w, _h) (FloatRect){ _x, _y, _w, _h };

static FloatRect get_bounding_box(const HitBox* hitbox, const Transform* transform)
{
  return (FloatRect){
    (transform->pos.x - hitbox->anchor.x),
    (transform->pos.y - hitbox->anchor.y),
    hitbox->size.x,
    hitbox->size.y,
  };
}

static BOOL has_intersection(const FloatRect* r1, const FloatRect* r2, Vec2* out)
{
  Vec2  half_size1;
  Vec2  half_size2;
  Vec2  center1;
  Vec2  center2;
  float dx, dy;

  half_size1 = VEC2(r1->w / 2.f, r1->h / 2.f);
  center1    = VEC2(r1->x + half_size1.x, r1->y + half_size1.y);

  half_size2 = VEC2(r2->w / 2.f, r2->h / 2.f);
  center2    = VEC2(r2->x + half_size2.x, r2->y + half_size2.y);

  dx = absf(center1.x - center2.x);
  dy = absf(center1.y - center2.y);

  out->x = dx - (half_size1.x + half_size2.x);
  out->y = dy - (half_size1.y + half_size2.y);

  return out->x < 0.f && out->y < 0.f;
}

static BOOL has_collision(FloatRect* obj_rect)
{
  FloatRect tile_rect;
  Vec2      out;

  int left_tile;
  int top_tile;
  int bottom_tile;
  int right_tile;


  tile_rect.w = TILE_SIZE;
  tile_rect.h = TILE_SIZE;

  left_tile   = (int)(obj_rect->x / TILE_SIZE);
  right_tile  = (int)((obj_rect->x + obj_rect->w) / TILE_SIZE);
  bottom_tile = (int)((obj_rect->y + obj_rect->h) / TILE_SIZE);
  top_tile    = (int)(obj_rect->y / TILE_SIZE);

  for (int row = top_tile; row <= bottom_tile; ++row)
  {
    for (int col = left_tile; col <= right_tile; ++col)
    {
      if (!map_is_floor(col, row))
      {
        tile_rect.x = col * TILE_SIZE;
        tile_rect.y = row * TILE_SIZE;
        if (has_intersection(&tile_rect, obj_rect, &out))
        {
          return TRUE;
        }
      }
    }
  }

  return FALSE;
}

void MapCollisionSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  HitBox*    hitboxs;
  Transform* transform;
  FloatRect  boundingbox;

  ecs_data(ecs, HITBOX, &entities, (void**)&hitboxs, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if (hitboxs[i].check_tile_collision)
    {
      transform   = ecs_get(ecs, entities[i], TRANSFORM);
      boundingbox = get_bounding_box(&hitboxs[i], transform);
      if (has_collision(&boundingbox))
      {
        transform->pos = transform->prev_pos;
      }
    }
  }
}
