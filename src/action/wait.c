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

static ActionVtbl _waitActionVTbl = {
  .start   = action_on_start,
  .end     = action_on_end,
  .update  = update,
  .cleanup = action_on_cleanup,
};

Action* wait_action_new(int duration)
{
  WaitAction* obj = action_alloc(WaitAction, &_waitActionVTbl);

  obj->duration = duration;
  return ACTION(obj);
}
