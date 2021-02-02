#include "system/debug/draw_hitbox.h"

#include "components.h"

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;



void hitbox_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  HitBox*       hitboxs;
  Transform*    transform;
  RECT rect;

  ecs_raw(g_ecs, HITBOX, &entities, (void**)&hitboxs, &cnt);

  SDL_SetRenderDrawColor(g_renderer, 0xff, 0x00, 0xff, 0xff);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);

    rect.x = transform->position.x - hitboxs[i].anchor.x - g_viewport.x;
    rect.y = transform->position.y - hitboxs[i].anchor.y - g_viewport.y;
    rect.w = hitboxs[i].size.x;
    rect.h = hitboxs[i].size.y;
    SDL_RenderDrawRect(g_renderer, &rect);
  }
}
