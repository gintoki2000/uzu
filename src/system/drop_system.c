#include "drop_system.h"
#include "mediator.h"
#include <components.h>
#include <entity_factory.h>
#include <item.h>

static ecs_entity_t (*create_fn_tbl[NUM_ITEMS])(Ecs*, Vec2) = {
  [ITEM_RED_FLASK]     = make_red_flask,
  [ITEM_BIG_RED_FLASK] = make_big_red_flask,
};

static void on_entity_died(Ecs* ecs, const SysEvt_EntityDied* event)
{
  Drop*      drop;
  Transform* transform;
  int        r;
  if ((drop = ecs_get(ecs, event->entity, DROP)) &&
      (transform = ecs_get(ecs, event->entity, TRANSFORM)))
  {
    r = rand() % 100;
    if (r <= drop->change1)
    {
      create_fn_tbl[drop->item1](ecs, transform->pos);
    }

    r = rand() % 100;
    if (r <= drop->change2)
    {
      create_fn_tbl[drop->item2](ecs, transform->pos);
    }
  }
}

void drop_system_init(Ecs* ecs)
{
  mediator_connect(SIG_ENTITY_DIED, ecs, SLOT(on_entity_died));
}
