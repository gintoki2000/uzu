#include "components.h"
#include "ecs/ecs.h"

  extern ecs_Registry*      gRegistry;
  extern RENDERER* gRenderer;
  extern RECT      gViewport;
void draw_destination_system()
{
  ecs_entity_t*    entities;
  ecs_size_t       cnt;
  POINT            p1, p2;

  WalkDirectlyToward* req;
  Transform*          transform;
  ecs_raw(gRegistry, WALK_DIRECTLY_TOWARD, &entities, (void**)&req, &cnt);
  SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0x00, 0xff);
  for (int i = 0; i < cnt; ++i)
  {
    transform    = ecs_get(gRegistry, entities[i], TRANSFORM);
    p1.x = transform->position.x;
    p1.y = transform->position.y;
    p2.x = req[i].destination.x;
    p2.y = req[i].destination.y;

    p1.x -= gViewport.x;
    p1.y -= gViewport.y;
    p2.x -= gViewport.x;
    p2.y -= gViewport.y;
    SDL_RenderDrawLine(gRenderer, p1.x, p1.y, p2.x, p2.y);
  }
}
