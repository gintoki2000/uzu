#include "draw_position.h"
#include <components.h>

extern SDL_Rect g_viewport;

void dbsys_draw_position_update(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transforms;

  ecs_data(ecs, TRANSFORM, &entities, (void**)&transforms, &cnt);

  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  for (int i = 0; i < cnt; ++i)
  {
    SDL_RenderDrawPoint(renderer,
                        transforms[i].pos.x - g_viewport.x,
                        transforms[i].pos.y - g_viewport.y);
  }
}
