#include "components.h"
#include "entity_factory.h"
#include "global.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

static ecs_entity_t (*const _pickupableMakeFnTbl[NUM_PICKUPABLE_TYPES])(Ecs*, Vec2, u8) = {
  [PICKUPABLE_RED_FLASK]     = make_red_flask,
  [PICKUPABLE_BIG_RED_FLASK] = make_big_red_flask,
  [PICKUPABLE_BLUE_FLASK]    = make_blue_flask,
  [PICKUPABLE_KEY_1_1]       = make_key_1_1,
  [PICKUPABLE_COIN]          = make_coin,
};

extern Ecs* gEcs;

static Vec2 get_random_speed()
{
  int   deg = rand() % 360;
  float rad = deg * 0.0174532925f;
  int   len = rand() % 50 + 100;
  Vec2  randomSpeed;
  randomSpeed.x = SDL_cosf(rad);
  randomSpeed.y = SDL_sinf(rad);
  randomSpeed.x *= len;
  randomSpeed.y *= len;
  return randomSpeed;
}

static void on_entity_died(SDL_UNUSED void* arg, const EntityDiedMsg* event)
{
  Drop*        drop;
  Transform*   transform;
  int          randomNumber;
  ecs_entity_t pickupable;
  BOOL         shouldDrop;
  Motion*      motion;



  if ((drop      = ecs_get(gEcs, event->entity, DROP      )) &&
      (transform = ecs_get(gEcs, event->entity, TRANSFORM)))
  {
    for (int i = 0; i < drop->cnt; ++i)
    {
      randomNumber = rand() % 100;
      shouldDrop   = randomNumber <= drop->rate[i];
      if (shouldDrop)
      {
        pickupable =
            _pickupableMakeFnTbl[drop->type[i]](gEcs, transform->position, drop->quality[i]);
        motion      = ecs_get(gEcs, pickupable, MOTION);
        motion->vel = get_random_speed();
        motion->vz  = 80.f;
      }
    }
  }
}

void drop_system_init()
{
  ems_connect(MSG_ENTITY_DIED, CALLBACK_2(on_entity_died));
}
