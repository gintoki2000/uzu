#include "draw_system.h"
#include <components.h>
#include <ecs/ecs.h>

void DrawSystem(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* ett;
  Visual*       visuals;
  Transform*    transform;
  ecs_size_t    cnt;
  SDL_Rect      dst;

  ecs_data(ecs, VISUAL, &ett, (void**)&visuals, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(ecs, ett[i], TRANSFORM))
      continue;
    transform = ecs_get(ecs, ett[i], TRANSFORM);
    dst.x = transform->x - visuals[i].anchor.x;
    dst.y = transform->y - visuals[i].anchor.y;
    dst.w = visuals[i].sprite.rect.w;
    dst.h = visuals[i].sprite.rect.h;
    SDL_RenderCopyEx(renderer,
                     visuals[i].sprite.tex,
                     &visuals[i].sprite.rect,
                     &dst,
                     transform->rot,
                     &visuals[i].anchor,
                     visuals[i].flip);
  }
}
