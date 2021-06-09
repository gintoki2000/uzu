#include "system/debug/draw_position.h"
#include <components.h>

extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;
extern ecs_Registry*          gRegistry;

void position_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transforms;

  ecs_raw(gRegistry, TRANSFORM, &entities, (void**)&transforms, &cnt);

  SDL_SetRenderDrawColor(gRenderer, 0, 255, 0, 255);
  for (int i = 0; i < cnt; ++i)
  {
    SDL_RenderDrawPoint(gRenderer,
                        transforms[i].position.x - gViewport.x,
                        transforms[i].position.y - gViewport.y);
  }
}
