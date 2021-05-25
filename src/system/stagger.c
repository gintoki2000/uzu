#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

static void on_stagger_added(SDL_UNUSED void* arg, const EcsComponentAdded* event)
{
  UnableToMove* unableToMove = ecs_get(gEcs, event->entity, UNABLE_TO_MOVE);
  if (unableToMove)
    unableToMove->count++;
  else
    ecs_set(gEcs, event->entity, UNABLE_TO_MOVE, &(UnableToMove){ 1 });
}

void stagger_system_init(void)
{
  signal_connect(ecs_on_construct(gEcs, STAGGER), CALLBACK_2(on_stagger_added));
}

void stagger_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Stagger* stagger;
  ecs_raw(gEcs, STAGGER, &entities, (void**)&stagger, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (stagger[i].duration > 0 && (--stagger[i].duration == 0))
    {
      ett_unable_to_move_pop(gEcs, entities[i]);
      ett_set_velocity(gEcs, entities[i], VEC2_ZERO);
      ecs_rmv(gEcs, entities[i], STAGGER);
    }
  }
}
