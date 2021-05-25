#include "action.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/event_messaging_sys.h"

extern Ecs* gEcs;

#define BUFF_SIZE 255
static ecs_entity_t _recentlyAdded[BUFF_SIZE];
static ecs_size_t   _recentlyAddedCount;

static void on_script_added(SDL_UNUSED void* arg, EcsComponentEvent* event)
{
  ASSERT(_recentlyAddedCount < BUFF_SIZE);
  _recentlyAdded[_recentlyAddedCount++] = event->entity;
}

static void start_recently_added_actions(void)
{
  Script* script;
  for (int i = 0; i < _recentlyAddedCount; ++i)
  {
    script = ecs_get(gEcs, _recentlyAdded[i], SCRIPT);
    ACTION_START(script->action, _recentlyAdded[i]);
  }
  _recentlyAddedCount = 0;
}

void action_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Script*       scripts;
  start_recently_added_actions();
  ecs_raw(gEcs, SCRIPT, &entities, (void**)&scripts, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (ACTION_UPDATE(scripts[i].action, entities[i]))
    {
      ACTION_END(scripts[i].action, entities[i]);
      INVOKE_EVENT(scripts[i].cbFinished, &scripts[i]);
      ecs_rmv(gEcs, entities[i], SCRIPT);
    }
  }
}

void action_system_init(void)
{
  signal_connect(ecs_on_construct(gEcs, SCRIPT), CALLBACK_2(on_script_added));
}
