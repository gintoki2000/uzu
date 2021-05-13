#include "components.h"
#include "ecs/ecs.h"

void draw_destination_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  extern Ecs*      g_ecs;
  extern RENDERER* g_renderer;
  extern RECT      g_viewport;
  POINT            p1, p2;

  WalkDirectlyToward* d;
  Transform*          t;
  ecs_raw(g_ecs, WALK_DIRECTLY_TOWARD, &entities, (void**)&d, &cnt);
  SDL_SetRenderDrawColor(g_renderer, 0xff, 0xff, 0x00, 0xff);
  for (int i = 0; i < cnt; ++i)
  {
    t    = ecs_get(g_ecs, entities[i], TRANSFORM);
    p1.x = t->position.x;
    p1.y = t->position.y;
    p2.x = d[i].destination.x;
    p2.y = d[i].destination.y;

    p1.x -= g_viewport.x;
    p1.y -= g_viewport.y;
    p2.x -= g_viewport.x;
    p2.y -= g_viewport.y;
    SDL_RenderDrawLine(g_renderer, p1.x, p1.y, p2.x, p2.y);
  }
}
