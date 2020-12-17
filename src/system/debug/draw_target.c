#include "draw_target.h"
#include "components.h"

extern SDL_Rect g_viewport;

void dbsys_mvto_target_update(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  btv_MoveDestination* dest;
  Transform*           tx;
  ecs_data(ecs, BTV_MOVE_DESTINATION, &entities, (void**)&dest, &cnt);
  SDL_SetRenderDrawColor(renderer, 3, 252, 119, 255);
  int x1, y1, x2, y2;
  for (int i = 0; i < cnt; ++i)
  {
    if ((tx = ecs_get(ecs, entities[i], TRANSFORM)))
    {
      x1 = tx->pos.x - g_viewport.x;
      y1 = tx->pos.y - g_viewport.y;
      x2 = dest[i].x - g_viewport.x;
      y2 = dest[i].y - g_viewport.y;
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
  }
}
