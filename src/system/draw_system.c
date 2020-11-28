#include "SDL_render.h"
#include <ecs/world.h>
#include <components.h>
#include <system/draw_system.h>

void DrawSystem(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* ett;
  Visual*       vs;
  Transform*    tx;
  ecs_size_t    cnt;
  SDL_Rect      dst;

  /*duyệt qua pool<Sprite> và lấy Transform bằng random access*/
  ecs_data(ecs, VISUAL, &ett, (void**)&vs, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if (!ecs_has(ecs, ett[i], TRANSFORM))
      continue;
    tx = ecs_get(ecs, ett[i], TRANSFORM);
    dst.x = tx->x; 
    dst.y = tx->y;
    dst.w = vs[i].sprite.rect.w;
    dst.h = vs[i].sprite.rect.h;
    SDL_RenderCopy(renderer, vs[i].sprite.tex, &vs[i].sprite.rect, &dst);
  }
}
