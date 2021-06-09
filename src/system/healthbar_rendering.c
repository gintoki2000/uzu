#include "components.h"
#include "ecs/ecs.h"
#include "system/rendering.h"

extern SDL_Rect      gViewport;
extern SDL_Renderer* gRenderer;
extern ecs_Registry*          gRegistry;


void healthbar_rendering_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  HealthBar* health_bar;
  Health*    health;
  float      p;
  SDL_Rect   border;
  SDL_Rect   inner;

  ecs_raw(gRegistry, HEAL_BAR, &entities, (void**)&health_bar, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(gRegistry, entities[i], TRANSFORM);
    health    = ecs_get(gRegistry, entities[i], HEALTH);
    if (health->current == health->max)
      continue;
    p = (float)health->current / (float)health->max;

    border.x = transform->position.x - health_bar[i].anchor.x - gViewport.x;
    border.y = transform->position.y - health_bar[i].anchor.y - gViewport.y;
    border.w = health_bar[i].len;
    border.h = 3;

    // SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    // SDL_RenderDrawRect(renderer, &border);

    inner.x = border.x + 1;
    inner.y = border.y + 1;
    inner.w = health_bar[i].len * p;
    inner.h = 1;
    SDL_SetRenderDrawColor(gRenderer,
                           health_bar[i].color.r,
                           health_bar[i].color.g,
                           health_bar[i].color.b,
                           health_bar->color.a);
    SDL_RenderFillRect(gRenderer, &inner);
  }
}
