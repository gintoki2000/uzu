#include "system/rendering_sys.h"
#include "components.h"
#include "ecs/ecs.h"
#include <stdlib.h>

extern SDL_Rect      g_viewport;
extern SDL_Renderer* g_renderer;
extern Ecs*          g_ecs;

typedef struct DrawCommand
{
  TEXTURE*         texture;
  RECT             src;
  RECT             dst;
  POINT            center;
  double           rotation;
  SDL_RendererFlip flip;
  COLOR            color;
  float            z;
} DrawCommand;

#define MAX_COMMAND_CNT 255
static DrawCommand _buff[MAX_COMMAND_CNT];
static u16         _count;

static int compare_draw_command(const DrawCommand* lhs, const DrawCommand* rhs)
{
  return (lhs->dst.y + lhs->dst.h) - (rhs->dst.y + rhs->dst.h);
}

static void sort()
{
  qsort(_buff, _count, sizeof(DrawCommand), (__compar_fn_t)compare_draw_command);
}

void rendering_system_update(void)
{
  ecs_entity_t* entities;
  Visual*       visuals;
  Transform*    transform;
  ecs_size_t    cnt;
  SDL_Rect      dst;

  _count = 0;

  ecs_raw(g_ecs, VISUAL, &entities, (void**)&visuals, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      dst.x = transform->position.x - visuals[i].anchor.x;
      dst.y = transform->position.y - visuals[i].anchor.y;
      dst.w = visuals[i].sprite.rect.w;
      dst.h = visuals[i].sprite.rect.h;
      if (SDL_HasIntersection(&g_viewport, &dst))
      {
        ASSERT(_count < MAX_COMMAND_CNT);
        dst.x -= g_viewport.x;
        dst.y -= g_viewport.y;
        _buff[_count++] = (DrawCommand){
          .texture  = visuals[i].sprite.tex,
          .src      = visuals[i].sprite.rect,
          .dst      = dst,
          .center   = visuals[i].anchor,
          .rotation = transform->rotation,
          .flip     = visuals[i].flip,
          .color    = visuals[i].color,
          .z        = transform->z,
        };
      }
    }
  }

  sort();

  for (int i = 0; i < _count; ++i)
  {
    dst = _buff[i].dst;
    dst.y -= _buff[i].z;
    SDL_SetTextureColorMod(_buff[i].texture, _buff[i].color.r, _buff[i].color.g, _buff[i].color.b);
    SDL_RenderCopyEx(g_renderer,
                     _buff[i].texture,
                     &_buff[i].src,
                     &dst,
                     _buff[i].rotation,
                     &_buff[i].center,
                     _buff[i].flip);
    SDL_SetTextureColorMod(_buff[i].texture, 0xff, 0xff, 0xff);
  }
}
