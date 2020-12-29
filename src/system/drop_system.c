#include "drop_system.h"
#include "mediator.h"
#include <components.h>
#include <entity_factory.h>
#include <item.h>

static ecs_entity_t (*create_fn_tbl[NUM_ITEM_TYPES])(Ecs*, Vec2) = {
  [ITEM_TYPE_RED_FLASK]     = make_red_flask,
  [ITEM_TYPE_BIG_RED_FLASK] = make_big_red_flask,
};

extern Ecs* g_ecs;

static void on_entity_died(void* arg, const SysEvt_EntityDied* event)
{
  (void)arg;
  Drop*      drop;
  Transform* transform;
  int        v;
  if ((drop = ecs_get(g_ecs, event->entity, DROP)) &&
      (transform = ecs_get(g_ecs, event->entity, TRANSFORM)))
  {
    if (v = rand() % 100, v <= drop->change1)
    {
      create_fn_tbl[drop->item1](g_ecs, transform->pos);
    }

    if (v = rand() % 100, v <= drop->change2)
    {
      create_fn_tbl[drop->item2](g_ecs, transform->pos);
    }
  }
}

void drop_system_init()
{
  mediator_connect(SYS_SIG_ENTITY_DIED, NULL, SLOT(on_entity_died));
}
