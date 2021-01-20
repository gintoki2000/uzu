#include "tile_collision_sys.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <constances.h>
#include <map.h>
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

INLINE RECT get_bounding_box(const HitBox* hitbox, const Transform* transform)
{
  return (RECT){
    (transform->pos.x - hitbox->anchor.x),
    (transform->pos.y - hitbox->anchor.y),
    hitbox->size.x,
    hitbox->size.y,
  };
}

INLINE int to_tile_coord(float v)
{
  return v / TILE_SIZE;
}

INLINE Vec2 to_world_coords(int tile_x, int tile_y)
{
  return (Vec2){ .x = (tile_x + 0.5f) * TILE_SIZE, .y = (tile_y + 0.5f) * TILE_SIZE };
}

#define V_AXIS 0
#define H_AXIS 1

INLINE get_axis(Vec2 prev_pos, Vec2 tile_pos)
{
  Vec2 d = vec2_sub(prev_pos, tile_pos);
  return absf(d.x) > absf(d.y) ? H_AXIS : V_AXIS;
}

void tile_collision_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  HitBox*    hitbox;
  Transform* transform;
  RECT       obj_rect;

  int   tile_x;
  int   tile_y;
  int   axis;
  float dx;
  float dy;
  BOOL  bAnyCollision;

  ecs_raw(g_ecs, TILE_COLLISION_TAG, &entities, NULL, &cnt);
  for (int ie = 0; ie < cnt; ++ie)
  {
    if ((transform = ecs_get(g_ecs, entities[ie], TRANSFORM)) &&
        (hitbox = ecs_get(g_ecs, entities[ie], HITBOX)))
    {
      obj_rect = get_bounding_box(hitbox, transform);

      bAnyCollision = FALSE;
      dx            = transform->pos.x - transform->prev_pos.x;
      dy            = transform->pos.y - transform->prev_pos.y;
      for (tile_x = to_tile_coord(obj_rect.x); tile_x <= to_tile_coord(obj_rect.x + obj_rect.w);
           ++tile_x)
      {
        tile_y = to_tile_coord(obj_rect.y + obj_rect.h);
        if (!map_is_floor(tile_x, tile_y))
        {
          bAnyCollision = TRUE;
          axis          = get_axis(transform->prev_pos, to_world_coords(tile_x, tile_y));

          if (axis == V_AXIS)
          {
            if (dy < 0.f)
              obj_rect.y = (tile_y + 1) * TILE_SIZE - obj_rect.h + 1;
            else
              obj_rect.y = tile_y * TILE_SIZE - obj_rect.h - 1;
          }
          else
          {
            if (dx < 0.f)
              obj_rect.x = (tile_x + 1) * TILE_SIZE;
            else
              obj_rect.x = tile_x * TILE_SIZE - obj_rect.w - 1;
          }
        }
      }
      if (bAnyCollision)
      {
        transform->pos.x = obj_rect.x + hitbox->anchor.x;
        transform->pos.y = obj_rect.y + hitbox->anchor.y;
      }
    }
  }
}
