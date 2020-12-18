#include "draw_target.h"
#include "components.h"
#include <SDL2/SDL2_gfxPrimitives.h>

extern SDL_Rect g_viewport;

void dbsys_mvto_target_update(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  btv_MoveDestination* dest;
  btv_FollowingTarget* target;
  Transform*           transform;
  Spot*                spot;
  ecs_data(ecs, BTV_MOVE_DESTINATION, &entities, (void**)&dest, &cnt);
  SDL_SetRenderDrawColor(renderer, 3, 252, 119, 255);
  int x1, y1, x2, y2;
  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(ecs, entities[i], TRANSFORM)))
    {
      x1 = transform->pos.x - g_viewport.x;
      y1 = transform->pos.y - g_viewport.y;
      x2 = dest[i].x - g_viewport.x;
      y2 = dest[i].y - g_viewport.y;
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
  }

  Transform* target_transform;
  ecs_data(ecs, BTV_FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(ecs, entities[i], TRANSFORM)) &&
        (target_transform = ecs_get(ecs, target[i], TRANSFORM)))
    {
      x1 = transform->pos.x - g_viewport.x;
      y1 = transform->pos.y - g_viewport.y;
      x2 = target_transform->pos.x - g_viewport.x;
      y2 = target_transform->pos.y - g_viewport.y;
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
  }

  int   x, y;
  float r;
  ecs_data(ecs, SPOT, &entities, (void**)&spot, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    x = spot[i].pos.x - g_viewport.x;
    y = spot[i].pos.y - g_viewport.y;
    r = spot[i].radius;

    aacircleColor(renderer, x, y, (Sint16)r, 0xffffffff); 
  }
}
