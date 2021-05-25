#include "components.h"
#include "entity_factory.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

struct hand_frame
{
  int    duration;
  double angle;
};

struct hand_anim
{
  const struct hand_frame* frames;
  Callback                 finished_callback;
  BOOL                     relative;
  Vec2                     initial_point;
  double                   initial_angle;
};

void weapon_shoot_system()
{
}
