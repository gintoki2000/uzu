#include "tile_collision_sys.h"

#include <components.h>
#include <constances.h>
#include <map.h>
#include <toolbox/toolbox.h>

INLINE RECT get_bounding_box(const HitBox* hitbox, const Transform* transform)
{
  return (RECT){
    (transform->pos.x - hitbox->anchor.x),
    (transform->pos.y - hitbox->anchor.y),
    hitbox->size.x,
    hitbox->size.y,
  };
}

static BOOL has_collision(const RECT* obj_rect, Vec2 vel)
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

void sys_tile_collision_update(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  HitBox*    hitbox;
  Transform* transform;
  RECT       boundingbox;

  ecs_data(ecs, TILE_COLLISION_TAG, &entities, NULL, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
        (hitbox = ecs_get(ecs, entities[i], HITBOX)))
    {
      boundingbox = get_bounding_box(hitbox, transform);
      if (has_collision(&boundingbox, (Vec2){0.f, 0.f}))
      {
        transform->pos = transform->prev_pos;
      }
    }
  }
}
