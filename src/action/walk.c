#include "action.h"
#include "components.h"
typedef struct WalkAction
{
  Action parent_instance;
  Vec2   destination;
  BOOL   completed;
} WalkAction;

extern Ecs* g_ecs;

static void walk_directly_toward_completed_callback(WalkAction* self, SDL_UNUSED BOOL result)
{
  self->completed = TRUE;
}

static void start(void* _self, ecs_entity_t target)
{
  WalkAction* self = _self;
  ASSERT(!ecs_has(g_ecs, target, WALK_DIRECTLY_TOWARD));
  ecs_set(g_ecs,
          target,
          WALK_DIRECTLY_TOWARD,
          &(WalkDirectlyToward){
              .destination = self->destination,
              .cbCompleted = CALLBACK_1(self, walk_directly_toward_completed_callback),
          });
}

static BOOL update(void* _self, SDL_UNUSED ecs_entity_t target)
{
  WalkAction* self = _self;
  return self->completed;
}

static void end(void* _self, ecs_entity_t target)
{
  WalkAction* self      = _self;
  Transform*  transform = ecs_get(g_ecs, target, TRANSFORM);
  transform->position   = self->destination;
}

static ActionVtbl _walk_action_vtbl = {
  .start   = start,
  .end     = end,
  .update  = update,
  .cleanup = action_default_cleanup_func,
};

Action* walk_action_new(Vec2 destination)
{
  WalkAction* action  = action_alloc(WalkAction, &_walk_action_vtbl);
  action->destination = destination;
  return ACTION(action);
}
