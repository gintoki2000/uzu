#include <components.h>
#include "healthbar_rendering_sys.h"
#include <ecs/ecs.h>

extern SDL_Rect g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs* g_ecs;

void healthbar_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  HealthBar* heal_bars;
  Health*     heath;
  float      p;
  SDL_Rect   border;
  SDL_Rect   inner;

  ecs_raw(g_ecs, HEAL_BAR, &entities, (void**)&heal_bars, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);
    heath     = ecs_get(g_ecs, entities[i], HEALTH);
    p         = (float)heath->hit_points / (float)heath->max_hit_points;

    border.x = transform->pos.x - heal_bars[i].anchor.x - g_viewport.x;
    border.y = transform->pos.y - heal_bars[i].anchor.y - g_viewport.y;
    border.w = heal_bars->len;
    border.h = 3;

    // SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    // SDL_RenderDrawRect(renderer, &border);

    inner.x = border.x + 1;
    inner.y = border.y + 1;
    inner.w = (border.w - 2) * p;
    inner.h = border.h - 2;
    SDL_SetRenderDrawColor(g_renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(g_renderer, &inner);
  }
}
