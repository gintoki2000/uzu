#include "chest_system.h"
#include "event_messaging_sys.h"
#include <components.h>
#include <constances.h>
#include <inventory.h>
#include <ui_msgbox.h>
#include <string.h>

extern Ecs* g_ecs;

static void open_chest(ecs_entity_t entity)
{
  Chest* chest;

  chest = ecs_get(g_ecs, entity, CHEST);

  if (chest->num_items == 0)
  {
    ui_msgbox_display("chest empty!");
  }
  else
  {
    for (int i = 0; i < chest->num_items; ++i)
    {
      add_to_inv(chest->items[i].type_id, chest->items[i].num_items);
    }
    chest->num_items = 0;
  }
}

static void on_command_selected(pointer_t arg, const MSG_CommandSelected* event)
{
  (void)arg;
  if (!strcmp(event->cmd, TEXT_COMMAND_OPEN) && ecs_has(g_ecs, event->entity, CHEST))
  {
    open_chest(event->entity);
  }
}

void chest_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, NULL, on_command_selected);
}
