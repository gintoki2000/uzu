#include "draw_target.h"
#include "components.h"
#include <SDL2/SDL2_gfxPrimitives.h>

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

void move_target_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Destination* dest;
  FollowingTarget* target;
  Transform*           transform;
  Spot*                spot;
  ecs_raw(g_ecs, DESTINATION, &entities, (void**)&dest, &cnt);
  SDL_SetRenderDrawColor(g_renderer, 3, 252, 119, 255);
  int x1, y1, x2, y2;
  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      x1 = transform->pos.x - g_viewport.x;
      y1 = transform->pos.y - g_viewport.y;
      x2 = dest[i].x - g_viewport.x;
      y2 = dest[i].y - g_viewport.y;
      SDL_RenderDrawLine(g_renderer, x1, y1, x2, y2);
    }
  }

  Transform* target_transform;
  ecs_raw(g_ecs, FOLLOWING_TARGET, &entities, (void**)&target, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)) &&
        (target_transform = ecs_get(g_ecs, target[i].entity, TRANSFORM)))
    {
      x1 = transform->pos.x - g_viewport.x;
      y1 = transform->pos.y - g_viewport.y;
      x2 = target_transform->pos.x - g_viewport.x;
      y2 = target_transform->pos.y - g_viewport.y;
      SDL_RenderDrawLine(g_renderer, x1, y1, x2, y2);

      aacircleColor(g_renderer, x2, y2, target[i].radius, 0xffffffff);
    }
  }

  int   x, y;
  float r;
  ecs_raw(g_ecs, SPOT, &entities, (void**)&spot, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    x = spot[i].pos.x - g_viewport.x;
    y = spot[i].pos.y - g_viewport.y;
    r = spot[i].radius;

    aacircleColor(g_renderer, x, y, (Sint16)r, 0xffffffff);
  }
}
