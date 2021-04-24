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
  .end     = action_default_end_func,
  .update  = action_default_update_func,
  .cleanup = action_default_cleanup_func,
};

Action* vanish_action_new(void)
{
  return action_alloc(VanishAction, &_vanish_action_vtbl);
}
