#ifndef ACTION_H
#define ACTION_H
#include "global.h"

typedef void (*ActionStartFunc)(void*, ecs_entity_t);
typedef void (*ActionEndFunc)(void*, ecs_entity_t);
typedef void (*ActionCleanupFunc)(void*);
typedef BOOL (*ActionTickFunc)(void*, ecs_entity_t);

typedef struct ActionVtbl
{
  void (*start)(void* self, ecs_entity_t target);
  void (*end)(void* self, ecs_entity_t target);
  BOOL (*update)(void* self, ecs_entity_t target);
  void (*cleanup)(void* self);
} ActionVtbl;

typedef struct Action
{
  const ActionVtbl* vtbl;
} Action;

void* _action_alloc(size_t size, const ActionVtbl* type);
#define action_alloc(T, vtbl) _action_alloc(sizeof(T), vtbl)
#define ACTION(ptr) ((Action*)ptr)
#define ACTION_START(action, target) ACTION(action)->vtbl->start(action, target)
#define ACTION_END(action, target) ACTION(action)->vtbl->end(action, target)
#define ACTION_UPDATE(action, target) ACTION(action)->vtbl->update(action, target)

void action_on_start(void* self, ecs_entity_t target);
void action_on_end(void* self, ecs_entity_t target);
BOOL action_on_update(void* self, ecs_entity_t target);
void action_on_cleanup(void* self);
void action_delete(Action* action);

typedef struct CompositeAction
{
  Action    parent_instance;
  PointerArray* children;
} CompositeAction;

#define COMPOSITE_ACTION(ptr) ((CompositeAction*)ptr)

CompositeAction* composite_action_init(CompositeAction* self);
void             composite_action_cleanup(void* self);
void             composite_action_add(CompositeAction* self, Action* action);

CompositeAction* parallel_action_new(void);
CompositeAction* sequence_action_new(void);
Action*          walk_action_new(Vec2 destination);
Action*          talk_action_new(u16 conversation_id);
Action*          express_emotion_action_new(int emoji, int duration);
Action*          wait_action_new(int duration);
Action*          vanish_action_new(void);
#endif // ACTION_H
