#include "hud_system.h"
#include <components.h>
#include <resources.h>

static const SDL_Rect _start = (SDL_Rect){ 0, 0, 4, 9 };

static const SDL_Rect _body = (SDL_Rect){ 4, 0, 3, 9 };

static const SDL_Rect _end = (SDL_Rect){ 7, 0, 5, 9 };

static const SDL_Color blood_color = (SDL_Color){ 237, 19, 41, 255 };

static const SDL_Color blood_lose = (SDL_Color){ 135, 11, 23, 255 };

#define UI_HEALTH_BAR_POSITION_X 10
#define UI_HEALTH_BAR_POSITION_Y 20

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;

static void draw_player_health_bar();

static void draw_player_health_bar()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  SDL_Texture*  texture;
  SDL_Rect      dst;

  Heath* health;

  texture = get_texture(TEX_PLAYER_HEALTH_BAR);

  ecs_raw(g_ecs, PLAYER_TAG, &entities, NULL, &cnt);

  if (cnt > 0)
  {

    health = ecs_get(g_ecs, entities[0], HEATH);

    dst.x = UI_HEALTH_BAR_POSITION_X;
    dst.y = UI_HEALTH_BAR_POSITION_Y;
    dst.w = _start.w;
    dst.h = _start.h;
    SDL_RenderCopy(g_renderer, texture, &_start, &dst);

    for (int i = 0; i < health->max_hit_points; ++i)
    {
      dst.x = UI_HEALTH_BAR_POSITION_X + _start.w + i * _body.w;
      dst.y = UI_HEALTH_BAR_POSITION_Y;
      dst.w = _body.w;
      dst.h = _body.h;
      SDL_RenderCopy(g_renderer, texture, &_body, &dst);
      SDL_SetRenderDrawColor(g_renderer, 255, 255, 255, 255);
    }

    dst.x = UI_HEALTH_BAR_POSITION_X + _start.w + health->max_hit_points * _body.w;
    dst.y = UI_HEALTH_BAR_POSITION_Y;
    dst.w = _end.w;
    dst.h = _end.h;
    SDL_RenderCopy(g_renderer, texture, &_end, &dst);

    for (int i = 0; i < health->max_hit_points; ++i)
    {
      dst.x = UI_HEALTH_BAR_POSITION_X + (_start.w + 1) + i * 3;
      dst.y = UI_HEALTH_BAR_POSITION_Y + 3;
      dst.w = 2;
      dst.h = 3;
      if (i < health->hit_points)
      {
        SDL_SetRenderDrawColor(g_renderer,
                               blood_color.r,
                               blood_color.g,
                               blood_color.b,
                               blood_color.a);
      }
      else
      {
        SDL_SetRenderDrawColor(g_renderer, blood_lose.r, blood_lose.g, blood_lose.b, blood_lose.a);
      }
      SDL_RenderFillRect(g_renderer, &dst);
    }
  }
}

void hub_system_update()
{
  draw_player_health_bar();
}
