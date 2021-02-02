#include "system/chest_system.h"
#include "system/event_messaging_sys.h"
#include "components.h"
#include "constances.h"
#include "inventory.h"
#include <string.h>
#include "ui_msgbox.h"

extern Ecs* g_ecs;

static void open_chest(ecs_entity_t entity)
{
  Chest*  chest;
  Visual* visual;

  chest = ecs_get(g_ecs, entity, CHEST);

  if (chest->num_items == 0)
  {
    ui_msgbox_display("chest empty!");
  }
  else
  {
    char msg[255] = "you got";
    char tmp[100];
    for (int i = 0; i < chest->num_items; ++i)
    {
      add_to_inv(chest->items[i].type_id, chest->items[i].num_items);
      sprintf(tmp, " %dx%s", chest->items[i].num_items, g_item_types[chest->items[i].type_id].name);
      strcat(msg, tmp);
    }
    ui_msgbox_display(msg);
    chest->num_items = 0;

    visual              = ecs_get(g_ecs, entity, VISUAL);
    visual->sprite.rect = RECT_CHEST_OPEN;
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
