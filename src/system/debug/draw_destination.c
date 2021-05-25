#include "components.h"
#include "ecs/ecs.h"

void draw_destination_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  extern Ecs*      gEcs;
  extern RENDERER* gRenderer;
  extern RECT      gViewport;
  POINT            p1, p2;

  WalkDirectlyToward* d;
  Transform*          t;
  ecs_raw(gEcs, WALK_DIRECTLY_TOWARD, &entities, (void**)&d, &cnt);
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0x00, 0xff);
  for (int i = 0; i < cnt; ++i)
  {
    t    = ecs_get(gEcs, entities[i], TRANSFORM);
    p1.x = t->position.x;
    p1.y = t->position.y;
    p2.x = d[i].destination.x;
    p2.y = d[i].destination.y;

    p1.x -= gViewport.x;
    p1.y -= gViewport.y;
    p2.x -= gViewport.x;
    p2.y -= gViewport.y;
    SDL_RenderDrawLine(gRenderer, p1.x, p1.y, p2.x, p2.y);
  }
}
