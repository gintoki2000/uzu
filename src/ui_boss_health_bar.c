#include "ui_boss_health_bar.h"
#include "components.h"
#include "constances.h"

static ecs_entity_t _target = ECS_NULL_ENT;
static BOOL         _visible;
static u32          _hit_points;
static u32          _max_health;
static u32          _damaged;
static u32          _elapsed_time;
extern Ecs*         gEcs;

#define MARGIN_BOTTOM (20)
#define HEIGHT (15)
#define POSITION_X (10)
#define POSITION_Y (WIN_WIDTH - HEIGHT - MARGIN_BOTTOM)

static const COLOR _background_color = { 0x00, 0x00, 0x00, 0xff };
static const COLOR _blood_color      = { 0xff, 0x33, 0x33, 0xd0 };
static const RECT  _background_bar   = { POSITION_X, POSITION_Y, HEIGHT, WIN_HEIGHT * 0.8 };

void ui_boss_health_bar_show(void)
{
  _visible = TRUE;
}

void ui_boss_health_bar_hide(void)
{
  _visible = FALSE;
}

void ui_boss_health_bar_update(void)
{
}

void ui_boss_health_bar_draw(void)
{
  Health* health;

  if (_target == ECS_NULL_ENT || !ecs_is_valid(gEcs, _target))
    return;

  health = ecs_get(gEcs, _target, HEALTH);
}
