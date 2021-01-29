#include "draw_position.h"
#include <components.h>

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

void position_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transforms;

  ecs_raw(g_ecs, TRANSFORM, &entities, (void**)&transforms, &cnt);

  SDL_SetRenderDrawColor(g_renderer, 0, 255, 0, 255);
  for (int i = 0; i < cnt; ++i)
  {
    SDL_RenderDrawPoint(g_renderer,
                        transforms[i].position.x - g_viewport.x,
                        transforms[i].position.y - g_viewport.y);
  }
}
