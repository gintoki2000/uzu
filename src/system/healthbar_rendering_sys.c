#include "healthbar_rendering_sys.h"
#include <components.h>
#include <ecs/ecs.h>

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

void healthbar_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  HealthBar* health_bar;
  Health*    health;
  float      p;
  SDL_Rect   border;
  SDL_Rect   inner;

  ecs_raw(g_ecs, HEAL_BAR, &entities, (void**)&health_bar, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(g_ecs, entities[i], TRANSFORM);
    health    = ecs_get(g_ecs, entities[i], HEALTH);
    if (health->hit_points == health->max_hit_points)
      continue;
    p = (float)health->hit_points / (float)health->max_hit_points;

    border.x = transform->position.x - health_bar[i].anchor.x - g_viewport.x;
    border.y = transform->position.y - health_bar[i].anchor.y - g_viewport.y;
    border.w = health_bar[i].len;
    border.h = 3;

    // SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    // SDL_RenderDrawRect(renderer, &border);

    inner.x = border.x + 1;
    inner.y = border.y + 1;
    inner.w = health_bar[i].len * p;
    inner.h = 1;
    SDL_SetRenderDrawColor(g_renderer,
    		health_bar[i].color.r,
			health_bar[i].color.g,
			health_bar[i].color.b,
			health_bar->color.a);
    SDL_RenderFillRect(g_renderer, &inner);
  }
}
