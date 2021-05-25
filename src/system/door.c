#include "components.h"
#include "constances.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

#include "inventory.h"
#include "toolbox/toolbox.h"
#include "ui_msgbox.h"

extern Ecs* gEcs;

//<-------------------event callbacks---------------------->//
static void on_command_selected(void* arg, const CommandSelectedMsg* event);
static void on_entity_collied_door(void* arg, const HitDoorMsg* event);
//==========================================================//

void door_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
  ems_connect(MSG_HIT_DOOR, CALLBACK_2(on_entity_collied_door));
}

static void on_command_selected(SDL_UNUSED void* arg, const CommandSelectedMsg* event)
{
  DoorAttributes* attrs;
  Visual*         visual;
  Interactable*   interactable;

  if ((attrs = ecs_get(gEcs, event->entity, DOOR_ATTRIBUTES)))
  {
    visual       = ecs_get(gEcs, event->entity, VISUAL);
    interactable = ecs_get(gEcs, event->entity, INTERACTABLE);
    if (strcmp(event->cmd, gCmdOpen) == 0)
    {
      if (attrs->requiredKey == DOOR_NO_REQUIRED_KEY || inv_has_item(attrs->requiredKey))
      {
        visual->sprite.rect       = gRectOpen;
        attrs->state              = DOOR_STATE_OPEN;
        interactable->commands[0] = gCmdClose;
      }
      else
      {
        ui_msgbox_display("it's locked!");
      }
    }
    else if (strcmp(event->cmd, gCmdClose) == 0)
    {
      visual->sprite.rect       = gRectClose;
      attrs->state              = DOOR_STATE_CLOSE;
      interactable->commands[0] = gCmdOpen;
    }
  }
}

static void on_entity_collied_door(SDL_UNUSED void* arg, const HitDoorMsg* event)
{

  DoorAttributes* door_info;
  HitBox *        eh, *dh;
  Transform *     etx, *dtx;

  RECT er;
  RECT dr;

  door_info = ecs_get(gEcs, event->door, DOOR_ATTRIBUTES);

  if (door_info->state == DOOR_STATE_OPEN)
    return;

  eh  = ecs_get(gEcs, event->entity, HITBOX);
  dh  = ecs_get(gEcs, event->door, HITBOX);
  etx = ecs_get(gEcs, event->entity, TRANSFORM);
  dtx = ecs_get(gEcs, event->door, TRANSFORM);

  er.x = etx->position.x - eh->anchor.x;
  er.y = etx->position.y - 5;
  er.w = eh->size.x;
  er.h = 5;

  dr.x = dtx->position.x - dh->anchor.x;
  dr.y = dtx->position.y - dh->anchor.y;
  dr.w = dh->size.x;
  dr.h = dh->size.y;
  if (SDL_HasIntersection(&er, &dr))
  {
    etx->position = etx->lastPosition;
  }
}
