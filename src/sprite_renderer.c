#include "sprite_renderer.h"
#include "constances.h"
#include "resources.h"

struct _Command
{
  u16    texture_id;
  RECT   src;
  RECT   dst;
  POINT  center;
  double rotation;
  FLIP   flip;
  COLOR  color;
  int    depth;
};

#define MAX_BUFFER_SIZE 512
static struct _Command _buff[MAX_BUFFER_SIZE];
static int             _cnt;
static BOOL            _is_drawing;

void sprite_renderer_begin()
{
  ASSERT(!_is_drawing);
  _is_drawing = TRUE;
  _cnt        = 0;
}

static int compare_depth(const struct _Command* lhs, const struct _Command* rhs)
{
  return lhs->depth - rhs->depth;
}

void sprite_renderer_end()
{
  extern RENDERER* g_renderer;
  ASSERT(_is_drawing);
  SDL_qsort(_buff, _cnt, sizeof(struct _Command), (int (*)(const void*, const void*))compare_depth);
  for (int i = 0; i < _cnt; ++i)
  {
    SDL_RenderCopyEx(g_renderer,
                     get_texture(_buff[i].texture_id),
                     &_buff[i].src,
                     &_buff[i].dst,
                     _buff[i].rotation,
                     &_buff[i].center,
                     _buff[i].flip);
  }
  _is_drawing = FALSE;
  _cnt        = 0;
}

void sprite_renderer_draw_ex(Sprite sprite,
                             POINT  position,
                             POINT  center,
                             double rotation,
                             FLIP   flip,
                             COLOR  color,
                             int    depth)
{
  ASSERT(_cnt < MAX_BUFFER_SIZE - 1);
  _buff[_cnt].texture_id = sprite.texture_id;
  _buff[_cnt].src        = sprite.rect;
  _buff[_cnt].dst        = (RECT){ position.x, position.y, sprite.rect.w, sprite.rect.h };
  _buff[_cnt].flip       = flip;
  _buff[_cnt].depth      = depth;
  _buff[_cnt].color      = color;
  _buff[_cnt].center     = center;
  _buff[_cnt].rotation   = rotation;
  _cnt++;
}

void sprite_renderer_draw(Sprite sprite, POINT position, int depth)
{
  sprite_renderer_draw_ex(sprite,
                          position,
                          (POINT){ sprite.rect.w / 2, sprite.rect.h / 2 },
                          0.0,
                          SDL_FLIP_NONE,
                          gColorWhite,
                          depth);
}
