#include "toolbox/toolbox.h"

static int  _camera_x;
static int  _camera_y;
static int  _duration;
static int  _magnitude;
extern RECT g_viewport;

void begin_sake_camera(int magnitude, int duration)
{
  _duration  = duration;
  _magnitude = magnitude;
}

void camera_shaker_update()
{
  _camera_x = g_viewport.x;
  _camera_y = g_viewport.y;
  if (_duration > 0)
  {
    --_duration;
    g_viewport.x += rand_range(-_magnitude, _magnitude);
    g_viewport.y += rand_range(-_magnitude, _magnitude);
  }
}

void camera_shaker_postupdate()
{
  g_viewport.x = _camera_x;
  g_viewport.y = _camera_y;
}
