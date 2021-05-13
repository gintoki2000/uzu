#include "action.h"
#include "components.h"
typedef struct VanishAction
{
  Action parent_instance;
} VanishAction;

static void start(SDL_UNUSED void* _self, ecs_entity_t target)
{
  extern Ecs* g_ecs;
  ecs_add(g_ecs, target, DESTROYED_TAG);
}

static ActionVtbl _vanish_action_vtbl = {
  .start   = start,
  .end     = action_on_end,
  .update  = action_on_update,
  .cleanup = action_on_cleanup,
};

Action* vanish_action_new(void)
{
  return action_alloc(VanishAction, &_vanish_action_vtbl);
}
