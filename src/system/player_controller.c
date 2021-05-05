#include "system/game_logic.h"

#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "engine/keyboard.h"
#include "entity_utils.h"
#include "inventory.h"
#include "resources.h"
#include "system/collision_sys.h"
#include "system/event_messaging_sys.h"
#include "ui_list.h"

extern Ecs* g_ecs;
extern RECT g_viewport;

#define INTERACTABLE_DISTANCE (TILE_SIZE * 1)
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3
#define MOVE_SPEED 100.f

ecs_entity_t g_curr_iteractable_entity = ECS_NULL_ENT;

//<--------------------------------event callbacks----------------------------------->//

static void on_list_select(pointer_t arg, const char* item);
static void on_level_unloaded(pointer_t arg, pointer_t event);

//<==================================================================================>//

static void on_list_select(SDL_UNUSED pointer_t arg, const char* item)
{
  ems_broadcast(MSG_COMANND_SELECTED,
                &(MSG_CommandSelected){
                    g_curr_iteractable_entity,
                    item,
                });
}

static void on_level_unloaded(pointer_t arg, pointer_t event)
{
  (void)arg;
  (void)event;
  g_curr_iteractable_entity = ECS_NULL_ENT;
}

struct CBPlayerInputProcess_QueryClosestInteracableArgs
{
  Vec2         player_position;
  float        closest_distance;
  ecs_entity_t closest_entity;
};

static BOOL
__callback_query_closest_interactable(struct CBPlayerInputProcess_QueryClosestInteracableArgs* args,
                                      ecs_entity_t                                             e)
{
  Vec2  entity_position = ett_get_position(g_ecs, e);
  float dist            = vec2_dist(args->player_position, entity_position);
  if (dist < args->closest_distance && ecs_has(g_ecs, e, INTERACTABLE))
  {
    args->closest_distance = dist;
    args->closest_entity   = e;
  }
  return TRUE;
}

static void find_interacable_entity()
{
  if (g_curr_iteractable_entity == ECS_NULL_ENT)
  {
    Vec2 player_pos;
    RECT rect;

    struct CBPlayerInputProcess_QueryClosestInteracableArgs cbargs;

    player_pos = snn_get_player_position(g_ecs);
    rect.x     = player_pos.x - INTERACTABLE_DISTANCE / 2;
    rect.y     = player_pos.y - INTERACTABLE_DISTANCE / 2;
    rect.w     = INTERACTABLE_DISTANCE;
    rect.h     = INTERACTABLE_DISTANCE;

    cbargs.player_position  = player_pos;
    cbargs.closest_entity   = ECS_NULL_ENT;
    cbargs.closest_distance = 9999.f;

    collision_box_query(&rect, 0xffff, CALLBACK_1(&cbargs, __callback_query_closest_interactable));
    g_curr_iteractable_entity = cbargs.closest_entity;
  }
  else
  {
    Vec2 player_pos, ientity_pos;

    player_pos  = snn_get_player_position(g_ecs);
    ientity_pos = ett_get_position(g_ecs, g_curr_iteractable_entity);

    if (vec2_dist(player_pos, ientity_pos) > INTERACTABLE_DISTANCE ||
        !ecs_has(g_ecs, g_curr_iteractable_entity, INTERACTABLE))
    {
      g_curr_iteractable_entity = ECS_NULL_ENT;
    }
  }
}

static void begin_interact(ecs_entity_t entity)
{
  Interactable* interactable = ecs_get(g_ecs, entity, INTERACTABLE);
  if (!interactable)
    return;
  ui_list_display((const char**)interactable->commands, interactable->num_commands);
  ui_list_set_pos(UI_LIST_POS_CENTER_X, UI_LIST_POS_CENTER_Y);
  ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_select));
  ems_broadcast(MSG_BEGIN_INTERACTION, &(MSG_BeginInteraction){ entity });
}

void player_process_input(SDL_UNUSED void* arg)
{

  ecs_entity_t player;
  Controller*  controller;

  if ((player = scn_get_player(g_ecs)) == ECS_NULL_ENT)
    return;
  if (ecs_has(g_ecs, player, SCRIPT))
    return;
  find_interacable_entity();

  controller = ecs_get(g_ecs, player, CONTROLLER);

  if (!ecs_has(g_ecs, player, INVULNERABLE))
  {

    controller->desired_direction = VEC2_ZERO;
    if (button_pressed(BUTTON_UP))
    {
      controller->desired_direction.y -= 1.f;
    }

    if (button_pressed(BUTTON_DOWN))
    {
      controller->desired_direction.y += 1.f;
    }

    if (button_pressed(BUTTON_LEFT))
    {
      controller->desired_direction.x -= 1.f;
    }

    if (button_pressed(BUTTON_RIGHT))
    {
      controller->desired_direction.x += 1.f;
    }

    if (button_just_pressed(BUTTON_OPEN_INVENTORY))
    {
      inventory_display();
      return;
    }

    if (mouse_button_just_pressed(SDL_BUTTON_LEFT) && !controller->in_action)
    {
      controller->action = CHARACTER_ACTION_REGULAR_ATK;
    }

    if (button_just_pressed(BUTTON_JUMP))
    {
      Transform* transform = ecs_get(g_ecs, player, TRANSFORM);
      if (transform->z <= 0.f)
      {
        Motion* motion = ecs_get(g_ecs, player, MOTION);
        motion->vz     = 100.f;
        Mix_PlayChannel(-1, get_sfx(SFX_MOV_JUMP), 0);
      }
    }

    if (button_just_pressed(BUTTON_INTERACT) && g_curr_iteractable_entity != ECS_NULL_ENT)
      begin_interact(g_curr_iteractable_entity);
  }
}

void player_controller_system_init(void)
{
  ems_connect(MSG_LEVEL_UNLOADED, CALLBACK_2(on_level_unloaded));
}
