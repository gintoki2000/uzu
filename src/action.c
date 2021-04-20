#include "action.h"
#include "config.h"

void action_delete(Action* action)
{
  if (action != NULL)
  {
    action->vtbl->cleanup(action);
    SDL_free(action);
  }
}

void action_default_cleanup_func(SDL_UNUSED void* self)
{
#if DEBUG
  INFO("action cleanup\n");
#endif
}

void action_default_start_func(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
#if DEBUG
  INFO("action start\n");
#endif
}

void action_default_end_func(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
}

BOOL action_default_update_func(SDL_UNUSED void* self, SDL_UNUSED ecs_entity_t target)
{
  return TRUE;
}

//==============================================================//
//  composite action section                                    //
//==============================================================//
CompositeAction* composite_action_init(CompositeAction* self)
{
  self->children = ptr_array_new(NULL);
  return self;
}

void composite_action_cleanup(void* _self)
{
  CompositeAction* self = _self;
  int              count;
  Action**         children;

  count    = self->children->cnt;
  children = ptr_array_storage(Action, self->children);
  for (int i = 0; i < count; ++i)
    action_delete(children[i]);
  ptr_array_delete(self->children);
  self->children = NULL;
}

void composite_action_add(CompositeAction* self, Action* action)
{
  ASSERT(action != NULL);
  ptr_array_add(self->children, action);
}
#include <stdarg.h>
void composite_action_addn(CompositeAction* self, int n, ...)
{
  va_list actions;
  va_start(actions, n);
  for (int i = 0; i < n; ++i)
  {
    ptr_array_add(self->children, va_arg(actions, Action*));
  }
}
//==============================================================//
//  parallel action section                                     //
//==============================================================//
typedef struct ParallelAction
{
  CompositeAction parent_instance;
} ParallelAction;

static ParallelAction* parallel_action_init(ParallelAction* self)
{
  composite_action_init(COMPOSITE_ACTION(self));
  return self;
}

static void parallel_action_start(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    ACTION_START(children[i], target);
  }
}

static void parallel_action_end(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    ACTION_END(children[i], target);
  }
}

static BOOL parallel_action_update(void* _self, ecs_entity_t target)
{
  Action** children;
  int      count;
  BOOL     finished = TRUE;

  children = ptr_array_storage(Action, COMPOSITE_ACTION(_self)->children);
  count    = COMPOSITE_ACTION(_self)->children->cnt;
  for (int i = 0; i < count; ++i)
  {
    if (!ACTION_UPDATE(children[i], target))
      finished = FALSE;
  }
  return finished;
}

static ActionVtbl _parallel_action_vtbl = { .start   = parallel_action_start,
                                            .end     = parallel_action_end,
                                            .update  = parallel_action_update,
                                            .cleanup = composite_action_cleanup };

Action* parallel_action_new(void)
{
  ParallelAction* action = SDL_malloc(sizeof(ParallelAction));
  ACTION(action)->vtbl   = &_parallel_action_vtbl;
  parallel_action_init(action);
  return ACTION(action);
}

//==============================================================//
//  sequence action section                                     //
//==============================================================//

typedef struct SequenceAction
{
  CompositeAction parent_instance;
  int             current_child_index;
} SequenceAction;

#define SEQUENCE_ACTION(ptr) ((SequenceAction*)ptr)

static SequenceAction* sequence_action_init(SequenceAction* self)
{
  composite_action_init(COMPOSITE_ACTION(self));
  self->current_child_index = -1;
  return self;
}

void static sequence_action_start(void* _self, ecs_entity_t target)
{
  SequenceAction*  self  = _self;
  CompositeAction* super = COMPOSITE_ACTION(_self);
  if (super->children->cnt == 0)
    return;

  self->current_child_index = 0;
  ACTION_START(ptr_array_at(super->children, 0), target);
}
void static sequence_action_end(void* self, SDL_UNUSED ecs_entity_t target)
{
  SEQUENCE_ACTION(self)->current_child_index = -1;
}

INLINE Action* sequence_action_next_child(SequenceAction* self)
{
  return self->current_child_index < COMPOSITE_ACTION(self)->children->cnt - 1
             ? ptr_array_at(COMPOSITE_ACTION(self)->children, ++self->current_child_index)
             : NULL;
}

static BOOL sequence_action_update(void* _self, ecs_entity_t target)
{
  SequenceAction* self = _self;
  Action *        current_child, *next_child;
  PtrArray*       children;

  children = COMPOSITE_ACTION(self)->children;
  if (children->cnt == 0)
    return TRUE;
  current_child = ptr_array_at(children, self->current_child_index);
  if (ACTION_UPDATE(current_child, target))
  {
    ACTION_END(current_child, target);
    if ((next_child = sequence_action_next_child(self)) != NULL)
      ACTION_START(next_child, target);
    else
      return TRUE;
  }
  return FALSE;
}

static ActionVtbl _sequence_action_vtbl = { .start   = sequence_action_start,
                                            .end     = sequence_action_end,
                                            .update  = sequence_action_update,
                                            .cleanup = composite_action_cleanup };

Action* sequence_action_new(void)
{
  SequenceAction* action = SDL_malloc(sizeof(SequenceAction));
  ACTION(action)->vtbl   = &_sequence_action_vtbl;
  sequence_action_init(action);
  return ACTION(action);
}
