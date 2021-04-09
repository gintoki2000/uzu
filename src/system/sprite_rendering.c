#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "sprite_renderer.h"
#include "system/game_logic.h"
#include <stdlib.h>

void rendering_system(void)
{
  extern SDL_Rect      g_viewport;
  extern SDL_Renderer* g_renderer;
  extern Ecs*          g_ecs;
  ecs_entity_t*        entities;
  Visual*              visuals;
  Transform*           transform;
  ecs_size_t           cnt;

  POINT position;
  int   depth;
  RECT  dst;

  ecs_raw(g_ecs, VISUAL, &entities, (void**)&visuals, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      position.x = transform->position.x - visuals[i].anchor.x;
      position.y = transform->position.y - visuals[i].anchor.y;
      depth      = position.y + visuals[i].sprite.rect.h;

      dst.x = position.x;
      dst.y = position.y;
      dst.w = visuals[i].sprite.rect.w;
      dst.h = visuals[i].sprite.rect.h;

      if (SDL_HasIntersection(&g_viewport, &dst))
      {
        position.x -= g_viewport.x;
        position.y -= g_viewport.y;
        sprite_renderer_draw_ex(visuals[i].sprite,
                                position,
                                visuals[i].anchor,
                                transform->rotation,
                                visuals[i].flip,
                                visuals[i].color,
                                depth);
      }
    }
  }
}
