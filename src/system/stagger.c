#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"

extern Ecs* g_ecs;

static void on_stagger_added(SDL_UNUSED void* arg, const EcsComponentAdded* event)
{
  UnableToMove* unable_to_move = ecs_get(g_ecs, event->entity, UNABLE_TO_MOVE);
  if (unable_to_move)
    unable_to_move->count++;
  else
    ecs_set(g_ecs, event->entity, UNABLE_TO_MOVE, &(UnableToMove){ 1 });
}

void stagger_system_init(void)
{
  signal_connect(ecs_on_construct(g_ecs, STAGGER), CALLBACK_2(on_stagger_added));
}

void stagger_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Stagger* stagger;
  ecs_raw(g_ecs, STAGGER, &entities, (void**)&stagger, &cnt);
  UnableToMove* unable_to_move;
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (stagger[i].duration > 0 && (--stagger[i].duration == 0))
    {
      unable_to_move = ecs_get(g_ecs, entities[i], UNABLE_TO_MOVE);
      ASSERT(unable_to_move && unable_to_move->count > 0);
      if (--unable_to_move->count == 0)
        ecs_rmv(g_ecs, entities[i], UNABLE_TO_MOVE);
      ett_set_velocity(g_ecs, entities[i], VEC2_ZERO);
      ecs_rmv(g_ecs, entities[i], STAGGER);
    }
  }
}
