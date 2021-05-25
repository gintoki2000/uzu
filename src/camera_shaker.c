#include "toolbox/toolbox.h"

static int  _camera_x;
static int  _camera_y;
static int  _duration;
static int  _magnitude;
extern RECT gViewport;

void begin_sake_camera(int magnitude, int duration)
{
  _duration  = duration;
  _magnitude = magnitude;
}

void camera_shaker_update()
{
  _camera_x = gViewport.x;
  _camera_y = gViewport.y;
  if (_duration > 0)
  {
    --_duration;
    gViewport.x += rand_range(-_magnitude, _magnitude);
    gViewport.y += rand_range(-_magnitude, _magnitude);
  }
}

void camera_shaker_postupdate()
{
  gViewport.x = _camera_x;
  gViewport.y = _camera_y;
}
