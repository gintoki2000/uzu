#include "draw_hitbox.h"

#include <components.h>

extern SDL_Rect g_viewport;
static void draw_rect(Rect* r, SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, 67, 63, 196, 0xff);
	SDL_RenderDrawLine(renderer, r->v1.x, r->v1.y, r->v2.x, r->v2.y);
	SDL_RenderDrawLine(renderer, r->v2.x, r->v2.y, r->v3.x, r->v3.y);
	SDL_RenderDrawLine(renderer, r->v3.x, r->v3.y, r->v4.x, r->v4.y);
	SDL_RenderDrawLine(renderer, r->v4.x, r->v4.y, r->v1.x, r->v1.y);
}

void dbsys_hitbox_update(Ecs* ecs, SDL_Renderer* renderer)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  HitBox*       hitboxs;
  Transform*    transform;
  Rect          rect;
  float         x, y, w, h;

  ecs_data(ecs, HITBOX, &entities, (void**)&hitboxs, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    transform = ecs_get(ecs, entities[i], TRANSFORM);

    x = transform->pos.x - hitboxs[i].anchor.x - g_viewport.x;
    y = transform->pos.y - hitboxs[i].anchor.y - g_viewport.y;
    w = hitboxs[i].size.x;
    h = hitboxs[i].size.y;
    rect_init_full(&rect, x, y, w, h, hitboxs[i].anchor.x, hitboxs[i].anchor.y, transform->rot);
	draw_rect(&rect, renderer);
  }
}
