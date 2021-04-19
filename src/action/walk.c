#include "action.h"
#include "components.h"
typedef struct WalkAction
{
  Action parent_instance;
  Vec2   destination;
} WalkAction;

extern Ecs* g_ecs;

static BOOL update(void* _self, ecs_entity_t target)
{
  WalkAction* self = _self;
  Controller* controller;
  Transform*  transform;
  Vec2        relative_position;

  controller = ecs_get(g_ecs, target, CONTROLLER);
  transform  = ecs_get(g_ecs, target, TRANSFORM);
  ASSERT_MSG(controller != NULL && transform != NULL,
             "require Transform & Controller components to perform walk action");
  relative_position = vec2_sub(self->destination, transform->position);
  if (vec2_mag2(relative_position) > 8.f)
  {
    controller->desired_direction = vec2_unit(relative_position);
    return FALSE;
  }
  return TRUE;
}

static void end(void* _self, ecs_entity_t target)
{
  WalkAction* self      = _self;
  Transform*  transform = ecs_get(g_ecs, target, TRANSFORM);
  transform->position   = self->destination;
}

static ActionVtbl _walk_action_vtbl = {
  .start   = action_default_start_func,
  .end     = end,
  .update  = update,
  .cleanup = action_default_cleanup_func,
};

Action* walk_action_new(Vec2 destination)
{
  WalkAction* action   = SDL_malloc(sizeof(WalkAction));
  ACTION(action)->vtbl = &_walk_action_vtbl;
  action->destination  = destination;
  return ACTION(action);
}
