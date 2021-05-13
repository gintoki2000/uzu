#include "system/game_logic.h"

#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "global.h"
#include "inventory.h"
#include "resources.h"
#include "system/event_messaging_sys.h"
#include "ui_helper.h"
#include "ui_quality.h"

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;

static u32          _current;
static ecs_entity_t _merchant = ECS_NULL_ENT;

#define SHOP_X 16
#define SHOP_Y 32
#define MAX_DISPLAYABLE 6
#define CELL_HEIGHT 18

//<-------------------------event callback------------------------>//

static void on_command_selected(pointer_t arg, const CommandSelectedMsg* event);
static void on_quality_selected(pointer_t arg, u32 value);

//<===============================================================>//

static void display_shop(ecs_entity_t entity);
static void draw_item(const ItemPayload* payload, s32 x, s32 y, BOOL selected);
static void process_input(void*);

void merchant_system_init()
{
  ems_connect(MSG_COMANND_SELECTED, CALLBACK_2(on_command_selected));
}

void merchant_system()
{
  if (_merchant == ECS_NULL_ENT)
    return;
  Merchant* merchant = ecs_get(g_ecs, _merchant, MERCHANT);

  RECT bg = { SHOP_X, SHOP_Y, 110, MAX_DISPLAYABLE * CELL_HEIGHT };

  draw_bordered_box(&bg, gUIColorBg, gUIColorBorder);

  for (u32 i = 0; i < merchant->numPayloads; ++i)
  {
    draw_item(&merchant->payloads[i], SHOP_X + 2, SHOP_Y + 2 + i * 17, i == _current);
  }
}

static void display_shop(ecs_entity_t entity)
{
  _merchant = entity;
  _current  = 0;
  input_push_state(INPUT_STATE_INST_1(process_input));
}

#define MAX_LENGTH_OF_ITEM_NAME 15

static char* item_name(ItemTypeId type_id, char name[MAX_LENGTH_OF_ITEM_NAME + 1])
{
  SDL_strlcpy(name, gItemTypes[type_id].name, MAX_LENGTH_OF_ITEM_NAME);
  return name;
}

static void draw_item(const ItemPayload* payload, s32 x, s32 y, BOOL selected)
{
  const ItemType* item_type = &gItemTypes[payload->type_id];
  FONT*           font      = get_font(FONT_DAMAGE_INDICATOR);

  RECT dst = { x, y, item_type->icon.rect.w, item_type->icon.rect.h };

  char name[MAX_LENGTH_OF_ITEM_NAME];

  SDL_RenderCopy(g_renderer, get_texture(item_type->icon.texture_id), &item_type->icon.rect, &dst);

  if (payload->available == MERCHANT_INIFINTE)
  {
    FC_DrawColor(font,
                 g_renderer,
                 x + dst.w + 4,
                 y + 5,
                 selected ? gUIColorTextSelected : gUIColorText,
                 "%s --- %3u$",
                 item_name(payload->type_id, name),
                 payload->price);
  }
  else
  {
    FC_DrawColor(font,
                 g_renderer,
                 x + dst.w + 4,
                 y + 5,
                 selected ? gUIColorTextSelected : gUIColorText,
                 "%s %3d %3u$",
                 item_name(payload->type_id, name),
                 payload->available,
                 payload->price);
  }
}

static void on_command_selected(pointer_t arg, const CommandSelectedMsg* event)
{
  (void)arg;
  if (strcmp(event->cmd, "BUY") == 0)
  {
    display_shop(event->entity);
  }
}

static void process_input(SDL_UNUSED void* arg)
{
  Merchant* merchant;
  if (_merchant == ECS_NULL_ENT)
    return;
  merchant = ecs_get(g_ecs, _merchant, MERCHANT);
  if (button_just_pressed(BUTTON_UP))
  {
    if (_current > 0)
      --_current;
  }

  if (button_just_pressed(BUTTON_DOWN))
  {
    if (_current < merchant->numPayloads - 1)
    {
      _current++;
    }
  }

  if (mouse_button_just_pressed(SDL_BUTTON_LEFT))
  {
    if (merchant->payloads[_current].available == MERCHANT_INIFINTE)
    {
      ui_quality_display("count:", 1, 1, 99);
      ui_quality_hook(UI_QUALITY_ON_SELECTED, CALLBACK_2(on_quality_selected));
    }
    else if (merchant->payloads[_current].available > 0)
    {
      ui_quality_display("count:", 1, 1, merchant->payloads[_current].available);
      ui_quality_hook(UI_QUALITY_ON_SELECTED, CALLBACK_2(on_quality_selected));
    }
  }

  if (mouse_button_just_pressed(SDL_BUTTON_RIGHT))
  {
    _merchant = ECS_NULL_ENT;
    input_pop_state();
  }
}

static void on_quality_selected(pointer_t arg, u32 value)
{
  (void)arg;
  Merchant*    merchant;
  ItemPayload* payload;

  merchant = ecs_get(g_ecs, _merchant, MERCHANT);

  payload = &merchant->payloads[_current];

  if (payload->available > 0)
  {
    payload->available -= value;
    inv_add_item(payload->type_id, value);
  }
}
