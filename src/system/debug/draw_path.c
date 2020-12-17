#include "draw_path.h"
#include <components.h>
#include <constances.h>
extern SDL_Rect g_viewport;

void dbsys_path_update(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  btv_Path*     paths;
  int           x1, y1, x2, y2;

  ecs_data(ecs, BTV_PATH, &entities, (void**)&paths, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    for (int j = 0; j < paths[i].cnt - 1; ++j)
    {
      x1 = (paths[i].nodes[j].x + 0.5) * TILE_SIZE - g_viewport.x;
      y1 = (paths[i].nodes[j].y + 0.5) * TILE_SIZE - g_viewport.y;

      x2 = (paths[i].nodes[j + 1].x + 0.5) * TILE_SIZE - g_viewport.x;
      y2 = (paths[i].nodes[j + 1].y + 0.5) * TILE_SIZE - g_viewport.y;

      if (j == paths[i].curr)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      else
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
      SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
    }
  }
}
