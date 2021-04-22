#include "action.h"

typedef struct WaitAction
{
  Action parent_instance;
  int    duration;
} WaitAction;

static BOOL update(void* _self, SDL_UNUSED ecs_entity_t target)
{
  WaitAction* self = _self;
  if (self->duration > 0)
    --self->duration;
  return self->duration == 0;
}

static ActionVtbl _wait_action_vtbl = {
  .size    = sizeof(WaitAction),
  .start   = action_default_start_func,
  .end     = action_default_end_func,
  .update  = update,
  .cleanup = action_default_cleanup_func,
};

Action* wait_action_new(int duration)
{
  WaitAction* obj = action_alloc(&_wait_action_vtbl);

  obj->duration = duration;
  return ACTION(obj);
}
