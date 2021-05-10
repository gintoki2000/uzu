#include "components.h"
#include "entity_factory.h"
#include "global.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

static ecs_entity_t (*const _pickupable_make_fn_tbl[NUM_PICKUPABLE_TYPES])(Ecs*, Vec2, u8) = {
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
  Vec2  random_speed;
  random_speed.x = SDL_cosf(rad);
  random_speed.y = SDL_sinf(rad);
  random_speed.x *= len;
  random_speed.y *= len;
  return random_speed;
}

static void on_entity_died(SDL_UNUSED void* arg, const EntityDiedMsg* event)
{
  Drop*        drop;
  Transform*   transform;
  int          random_value;
  ecs_entity_t pickupable;
  BOOL         should_drop;
  Motion*      pickupable_motion;

  if ((drop      = ecs_get(g_ecs, event->entity, DROP      )) &&
      (transform = ecs_get(g_ecs, event->entity, TRANSFORM)))
  {
    for (int i = 0; i < drop->cnt; ++i)
    {
      random_value = rand() % 100;
      should_drop  = random_value <= drop->rate[i];
      if (should_drop)
      {
        pickupable =
            _pickupable_make_fn_tbl[drop->type[i]](g_ecs, transform->position, drop->quality[i]);
        pickupable_motion      = ecs_get(g_ecs, pickupable, MOTION);
        pickupable_motion->vel = get_random_speed();
        pickupable_motion->vz  = 80.f;
      }
    }
  }
}

void drop_system_init()
{
  ems_connect(MSG_ENTITY_DIED, CALLBACK_2(on_entity_died));
}
