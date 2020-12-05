#include "map_collision_system.h"
#include <components.h>
#include <constances.h>
#include <map.h>
#include <toolbox/toolbox.h>

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


static BOOL has_collision(FloatRect* obj_rect)
{

  int left_tile;
  int top_tile;
  int bottom_tile;
  int right_tile;

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
        return TRUE;
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
