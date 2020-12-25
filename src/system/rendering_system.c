#include "rendering_system.h"
#include <components.h>
#include <ecs/ecs.h>

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

void update_rendering_system(void)
{
  ecs_entity_t* entities;
  Visual*       visuals;
  Transform*    transform;
  ecs_size_t    cnt;
  SDL_Rect      dst;

  ecs_raw(g_ecs, VISUAL, &entities, (void**)&visuals, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      dst.x = transform->pos.x - visuals[i].anchor.x;
      dst.y = transform->pos.y - visuals[i].anchor.y;
      dst.w = visuals[i].sprite.rect.w;
      dst.h = visuals[i].sprite.rect.h;
      if (SDL_HasIntersection(&g_viewport, &dst))
      {
        dst.x -= g_viewport.x;
        dst.y -= g_viewport.y;
        SDL_RenderCopyEx(g_renderer,
                         visuals[i].sprite.tex,
                         &visuals[i].sprite.rect,
                         &dst,
                         transform->rot,
                         &visuals[i].anchor,
                         visuals[i].flip);
      }
    }
  }
}
