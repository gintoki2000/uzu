#include "door_system.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <constances.h>

#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

//<-------------------event callbacks---------------------->//
static void on_command_selected(void* arg, const MSG_CommandSelected* event);
//==========================================================//

void door_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, NULL, on_command_selected);
}

static void on_command_selected(SDL_UNUSED void* arg, const MSG_CommandSelected* event)
{

  DoorInfo*     door_info;
  Visual*       visual;
  Interactable* interactable;

  if ((door_info = ecs_get(g_ecs, event->entity, DOOR_INFO)))
  {
    if (strcmp(event->cmd, TEXT_COMMAND_OPEN) == 0)
    {
      visual                    = ecs_get(g_ecs, event->entity, VISUAL);
      interactable              = ecs_get(g_ecs, event->entity, INTERACTABLE);
      visual->sprite.rect       = RECT_DOOR_OPEN;
      door_info->state          = DOOR_STATE_OPEN;
      interactable->commands[0] = TEXT_COMMAND_CLOSE;
    }
    else if (strcmp(event->cmd, TEXT_COMMAND_CLOSE) == 0)
    {
      visual                    = ecs_get(g_ecs, event->entity, VISUAL);
      interactable              = ecs_get(g_ecs, event->entity, INTERACTABLE);
      visual->sprite.rect       = RECT_DOOR_CLOSE;
      door_info->state          = DOOR_STATE_CLOSE;
      interactable->commands[0] = TEXT_COMMAND_OPEN;
    }
  }
}
