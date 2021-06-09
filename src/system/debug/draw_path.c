#include "system/debug/draw_path.h"
#include <components.h>
#include <constances.h>
extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;
extern ecs_Registry*          gRegistry;

void draw_path_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Path*         paths;
  int           x1, y1, x2, y2;

  ecs_raw(gRegistry, PATH, &entities, (void**)&paths, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    for (int j = 0; j < paths[i].count - 1; ++j)
    {
      x1 = (paths[i].nodes[j].x + 0.5) * TILE_SIZE - gViewport.x;
      y1 = (paths[i].nodes[j].y + 0.5) * TILE_SIZE - gViewport.y;

      x2 = (paths[i].nodes[j + 1].x + 0.5) * TILE_SIZE - gViewport.x;
      y2 = (paths[i].nodes[j + 1].y + 0.5) * TILE_SIZE - gViewport.y;

      SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
      SDL_RenderDrawLine(gRenderer, x1, y1, x2, y2);
    }
  }
}
