#include "system/debug/draw_hitbox.h"

#include "components.h"

extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;
extern Ecs*          gEcs;



void hitbox_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  HitBox*       hitboxs;
  Transform*    transform;
  RECT rect;

  ecs_raw(gEcs, HITBOX, &entities, (void**)&hitboxs, &cnt);

  SDL_SetRenderDrawColor(gRenderer, 0xff, 0x00, 0xff, 0xff);
  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(gEcs, entities[i], TRANSFORM);

    rect.x = transform->position.x - hitboxs[i].anchor.x - gViewport.x;
    rect.y = transform->position.y - hitboxs[i].anchor.y - gViewport.y;
    rect.w = hitboxs[i].size.x;
    rect.h = hitboxs[i].size.y;
    SDL_RenderDrawRect(gRenderer, &rect);
  }
}
