#include "ui_subtile.h"
#include "constances.h"
#include "resources.h"
#include "toolbox/toolbox.h"

#define INTERVAL 120
#define POSITION_X (WIN_WIDTH / 2)
#define POSITION_Y (WIN_HEIGHT - 30)

extern RENDERER* gRenderer;

static const char** _items;
static u16          _ticks;
static FONT*        _font; // cache font

static const FC_Effect TEXT_EFFECT = { .alignment = FC_ALIGN_CENTER,
                                       .color     = { 0xff, 0xff, 0xff, 0xff },
                                       .scale     = { 1.f, 1.f } };

void ui_subtile_init()
{
  _font = get_font(FONT_ITEM_PICKED_UP);
}

void ui_subtile_fini()
{
}

void ui_subtile_update()
{
  if (_items == NULL)
    return;

  if (*_items != NULL)
  {
    if (++_ticks >= INTERVAL)
    {
      ++_items;
      _ticks = 0;
    }
  }
  else
  {
    _items = NULL;
  }
}

void ui_subtile_draw()
{
  if (_items != NULL && *_items != NULL)
    FC_DrawEffect(_font, gRenderer, POSITION_X, POSITION_Y, TEXT_EFFECT, "%s",*_items);
}

void ui_subtile_show(const char* items[])
{
  _ticks = 0;
  _items = items;
}
