#include "drop_system.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <entity_factory.h>
#include <types.h>

static ecs_entity_t (*create_fn_tbl[NUM_ITEM_TYPES])(Ecs*, Vec2) = {
  [ITEM_TYPE_RED_FLASK]     = make_red_flask,
  [ITEM_TYPE_BIG_RED_FLASK] = make_big_red_flask,
  [ITEM_TYPE_BLUE_FLASK]    = make_blue_flask,
  [ITEM_TYPE_KEY_1_1]       = make_key_1_1,
};

extern Ecs* g_ecs;

static void on_entity_died(void* arg, const MSG_EntityDied* event)
{
  (void)arg;
  Drop*      drop;
  Transform* transform;
  int        v;
  if ((drop = ecs_get(g_ecs, event->entity, DROP)) &&
      (transform = ecs_get(g_ecs, event->entity, TRANSFORM)))
  {
    v = rand() % 100;
    if (v <= drop->change1)
    {
      create_fn_tbl[drop->item1](g_ecs, transform->position);
    }

    v = rand() % 100;
    if (v <= drop->change2)
    {
      create_fn_tbl[drop->item2](g_ecs, transform->position);
    }
  }
}

void drop_system_init()
{
  ems_connect(MSG_ENTITY_DIED, NULL, on_entity_died);
}
