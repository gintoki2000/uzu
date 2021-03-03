#include "components.h"
#include "constances.h"
#include "inventory.h"
#include "system/event_messaging_sys.h"
#include "system/game_logic.h"
#include "ui_msgbox.h"
#include "ui_msgbox_w_icon.h"
#include <string.h>

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
    UI_MsgBoxWIcon_Entry entries[chest->num_items];
    for (int i = 0; i < chest->num_items; ++i)
    {
      add_to_inv(chest->items[i].type_id, chest->items[i].num_items);
      SDL_strlcpy(entries[i].text,
                  g_item_types[chest->items[i].type_id].name,
                  UI_MSGBOX_W_ICON_MAX_TEXT_LEN);
      entries[i].icon = g_item_types[chest->items[i].type_id].icon;
    }
    ui_msgbox_w_icon_show(entries, chest->num_items);
    chest->num_items    = 0;
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
