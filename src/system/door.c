#include "components.h"
#include "constances.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"

#include "inventory.h"
#include "toolbox/toolbox.h"
#include "ui_msgbox.h"

extern Ecs* g_ecs;

//<-------------------event callbacks---------------------->//
static void on_command_selected(void* arg, const MSG_CommandSelected* event);
static void on_entity_collied_door(void* arg, const MSG_HitDoor* event);
//==========================================================//

void door_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
  ems_connect(MSG_HIT_DOOR, CALLBACK_2(on_entity_collied_door));
}

static void on_command_selected(SDL_UNUSED void* arg, const MSG_CommandSelected* event)
{
  DoorAttributes* attrs;
  Visual*         visual;
  Interactable*   interactable;

  if ((attrs = ecs_get(g_ecs, event->entity, DOOR_ATTRIBUTES)))
  {
    visual       = ecs_get(g_ecs, event->entity, VISUAL);
    interactable = ecs_get(g_ecs, event->entity, INTERACTABLE);
    if (strcmp(event->cmd, TEXT_COMMAND_OPEN) == 0)
    {
      if (attrs->required_key == DOOR_NO_REQUIRED_KEY || has_item_in_inventory(attrs->required_key))
      {
        visual->sprite.rect       = RECT_DOOR_OPEN;
        attrs->state              = DOOR_STATE_OPEN;
        interactable->commands[0] = TEXT_COMMAND_CLOSE;
      }
      else
      {
        ui_msgbox_display("it's locked!");
      }
    }
    else if (strcmp(event->cmd, TEXT_COMMAND_CLOSE) == 0)
    {
      visual->sprite.rect       = RECT_DOOR_CLOSE;
      attrs->state              = DOOR_STATE_CLOSE;
      interactable->commands[0] = TEXT_COMMAND_OPEN;
    }
  }
}

static void on_entity_collied_door(SDL_UNUSED void* arg, const MSG_HitDoor* event)
{

  DoorAttributes* door_info;
  HitBox *        eh, *dh;
  Transform *     etx, *dtx;

  RECT er;
  RECT dr;

  door_info = ecs_get(g_ecs, event->door, DOOR_ATTRIBUTES);

  if (door_info->state == DOOR_STATE_OPEN)
    return;

  eh  = ecs_get(g_ecs, event->entity, HITBOX);
  dh  = ecs_get(g_ecs, event->door, HITBOX);
  etx = ecs_get(g_ecs, event->entity, TRANSFORM);
  dtx = ecs_get(g_ecs, event->door, TRANSFORM);

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
    etx->position = etx->prev_position;
  }
}
