#include "system/game_logic.h"
#include "components.h"
#include "entity_factory.h"
#include "global.h"
#include "system/event_messaging_sys.h"

static ecs_entity_t (*create_fn_tbl[NUM_PICKUPABLE_TYPES])(Ecs*, Vec2) = {
  [PICKUPABLE_RED_FLASK]     = make_red_flask,
  [PICKUPABLE_BIG_RED_FLASK] = make_big_red_flask,
  [PICKUPABLE_BLUE_FLASK]    = make_blue_flask,
  [PICKUPABLE_KEY_1_1]       = make_key_1_1,
  [PICKUPABLE_COIN]          = make_coin,
};

extern Ecs* g_ecs;

static Vec2 get_random_speed()
{
  int   deg = rand() % 360;
  float rad = deg * 0.0174532925f;
  int   len = rand() % 50 + 100;
  Vec2  v;
  v.x = SDL_cosf(rad);
  v.y = SDL_sinf(rad);
  v.x *= len;
  v.y *= len;
  return v;
}

static void on_entity_died(SDL_UNUSED void* arg, const MSG_EntityDied* event)
{
  Drop*        drop;
  Transform*   transform;
  int          v;
  ecs_entity_t pickupable = ECS_NULL_ENT;
  if ((drop = ecs_get(g_ecs, event->entity, DROP)) &&
      (transform = ecs_get(g_ecs, event->entity, TRANSFORM)))
  {
    v = rand() % 100;
    if (v <= drop->change1)
    {
      pickupable = create_fn_tbl[drop->item1](g_ecs, transform->position);
      if (pickupable != ECS_NULL_ENT)
      {
        Motion* motion = ecs_get(g_ecs, pickupable, MOTION);
        motion->vel    = get_random_speed();
        motion->vz     = 60.f;
      }
    }

    v = rand() % 100;
    if (v <= drop->change2)
    {
      pickupable = create_fn_tbl[drop->item2](g_ecs, transform->position);
      if (pickupable != ECS_NULL_ENT)
      {
        Motion* motion = ecs_get(g_ecs, pickupable, MOTION);
        motion->vel    = get_random_speed();
        motion->vz     = 60.f;
      }
    }
    pickupable     = make_coin(g_ecs, transform->position);
    Motion* motion = ecs_get(g_ecs, pickupable, MOTION);
    motion->vel    = get_random_speed();
    motion->vz     = 60.f;
  }
}

void drop_system_init()
{
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
}
