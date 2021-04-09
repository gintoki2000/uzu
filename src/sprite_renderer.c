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
static struct _Command _buffer[MAX_BUFFER_SIZE];
static int             _command_count;
static BOOL            _is_drawing;

void sprite_renderer_begin()
{
  ASSERT(!_is_drawing);
  _is_drawing = TRUE;
  _command_count = 0;
}

static int compare_depth(const struct _Command* lhs, const struct _Command* rhs)
{
  return lhs->depth - rhs->depth;
}

void sprite_renderer_end()
{
  extern RENDERER* g_renderer;
  ASSERT(_is_drawing);
  _is_drawing = FALSE;
  SDL_qsort(_buffer,
            _command_count,
            sizeof(struct _Command),
            (int (*)(const void*, const void*))compare_depth);
  for (int i = 0; i < _command_count; ++i)
  {
    SDL_RenderCopyEx(g_renderer,
                     get_texture(_buffer[i].texture_id),
                     &_buffer[i].src,
                     &_buffer[i].dst,
                     _buffer[i].rotation,
                     &_buffer[i].center,
                     _buffer[i].flip);
  }
  _command_count = 0;
}

void sprite_renderer_draw_ex(Sprite sprite,
                             POINT  position,
                             POINT  center,
                             double rotation,
                             FLIP   flip,
                             COLOR  color,
                             int    depth)
{
  ASSERT(_command_count < MAX_BUFFER_SIZE - 1);
  _buffer[_command_count].texture_id = sprite.texture_id;
  _buffer[_command_count].src        = sprite.rect;
  _buffer[_command_count].dst      = (RECT){ position.x, position.y, sprite.rect.w, sprite.rect.h };
  _buffer[_command_count].flip     = flip;
  _buffer[_command_count].depth    = depth;
  _buffer[_command_count].color    = color;
  _buffer[_command_count].center   = center;
  _buffer[_command_count].rotation = rotation;
  _command_count++;
}

void sprite_renderer_draw(Sprite sprite, POINT position, int depth)
{
  sprite_renderer_draw_ex(sprite,
                          position,
                          (POINT){ sprite.rect.w / 2, sprite.rect.h / 2 },
                          0.0,
                          SDL_FLIP_NONE,
                          COLOR_WHITE,
                          depth);
}
