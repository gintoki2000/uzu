#include "action.h"
#include "components.h"
#include "ecs/ecs.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;

#define BUFF_SIZE 255
static ecs_entity_t _recently_added[BUFF_SIZE];
static ecs_size_t   _recently_added_count;

static void on_script_added(SDL_UNUSED void* arg, EcsComponentEvent* event)
{
  ASSERT(_recently_added_count < BUFF_SIZE);
  _recently_added[_recently_added_count++] = event->entity;
}

static void start_recently_added_actions(void)
{
  Script* script;
  for (int i = 0; i < _recently_added_count; ++i)
  {
    script = ecs_get(g_ecs, _recently_added[i], SCRIPT);
    ACTION_START(script->action, _recently_added[i]);
  }
  _recently_added_count = 0;
}

void action_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Script*       scripts;
  start_recently_added_actions();
  ecs_raw(g_ecs, SCRIPT, &entities, (void**)&scripts, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if (ACTION_UPDATE(scripts[i].action, entities[i]))
    {
      ACTION_END(scripts[i].action, entities[i]);
      INVOKE_EVENT(scripts[i].finished_callback, &scripts[i]);
      ecs_rmv(g_ecs, entities[i], SCRIPT);
    }
  }
}

void action_system_init(void)
{
  ecs_connect(g_ecs, ECS_EVT_ADD_COMP, SCRIPT, CALLBACK_2(on_script_added));
}
