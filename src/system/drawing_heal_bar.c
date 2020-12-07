#include "drawing_heal_bar.h"
#include <components.h>

extern SDL_Rect g_viewport;
void            DrawingHealBarSystem(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Transform* transform;
  HealBar*   heal_bars;
  Heath*     heath;
  float      p;
  SDL_Rect   border;
  SDL_Rect   inner;

  ecs_data(ecs, HEAL_BAR, &entities, (void**)&heal_bars, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);
    heath     = ecs_get(ecs, entities[i], HEATH);
    p         = (float)heath->hit_points / (float)heath->max_hit_points;

    border.x = transform->pos.x - heal_bars[i].anchor.x - g_viewport.x;
    border.y = transform->pos.y - heal_bars[i].anchor.y - g_viewport.y;
    border.w = heal_bars->len;
    border.h = 3;

    //SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255);
    //SDL_RenderDrawRect(renderer, &border);

    inner.x = border.x + 1;
    inner.y = border.y + 1;
    inner.w = (border.w - 2) * p;
    inner.h = border.h - 2;
    SDL_SetRenderDrawColor(renderer, 0, 200, 0, 255);
    SDL_RenderFillRect(renderer, &inner);
  }
}
