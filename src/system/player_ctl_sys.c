#include "player_ctl_sys.h"
#include "collision_sys.h"
#include "constances.h"
#include "inventory.h"
#include "mediator.h"
#include "ui_list.h"

#include <components.h>
#include <ecs/ecs.h>
#include <engine/keyboard.h>
#include <utils.h>

extern Ecs* g_ecs;

#define INTERACTABLE_DISTANCE TILE_SIZE * 4
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3

ecs_entity_t g_curr_iteractable_entity = ECS_NULL_ENT;

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

//<--------------------------------event callbacks----------------------------------->//

static void on_list_select(pointer_t arg, const char* item);

//<==================================================================================>//

static void on_list_select(pointer_t arg, const char* item)
{
  (void)arg;
  INFO("you selected %s\n", item);

  mediator_broadcast(SYS_SIG_EXEC_INTERACTION_CMD,
                     &(SysEvt_ExecInteractionCmd){
                         g_curr_iteractable_entity,
                         item,
                     });
}

static void update_pointed_entity()
{
  if (g_curr_iteractable_entity == ECS_NULL_ENT)
  {
    Vec2         player_pos, ientity_pos;
    RECT         rect;
    ecs_entity_t entities[5];
    u16          cnt;
    float        closest_distance;
    float        dist;
    ecs_entity_t closest;

    player_pos = get_player_position(g_ecs);
    rect.x     = player_pos.x - TILE_SIZE * 2;
    rect.y     = player_pos.y - TILE_SIZE * 2;
    rect.w     = TILE_SIZE * 3;
    rect.h     = TILE_SIZE * 4;

    collision_system_query_ex(&rect, BIT(CATEGORY_INTERACABLE), entities, &cnt, 5);

    if (cnt > 0)
    {
      closest          = entities[0];
      closest_distance = vec2_mag(vec2_sub(player_pos, get_entity_position(g_ecs, entities[0])));
      for (int i = 1; i < cnt; ++i)
      {
        ientity_pos = get_entity_position(g_ecs, entities[i]);
        if ((dist = vec2_mag(vec2_sub(ientity_pos, player_pos))) < closest_distance)
        {
          closest_distance = dist;
          closest          = entities[i];
        }
      }
      g_curr_iteractable_entity = closest;
    }
  }
  else
  {
    Vec2 player_pos, ientity_pos;

    player_pos  = get_player_position(g_ecs);
    ientity_pos = get_entity_position(g_ecs, g_curr_iteractable_entity);

    if (vec2_mag(vec2_sub(player_pos, ientity_pos)) > INTERACTABLE_DISTANCE)
    {
      g_curr_iteractable_entity = ECS_NULL_ENT;
    }
  }
}

static void begin_interact(ecs_entity_t entity)
{
  Interactable* interactable;
  char*         items[5];
  INFO("begin interact with e" ECS_ENT_FMT_PATTERN "\n", ECS_ENT_FMT_VARS(entity));

  mediator_broadcast(SYS_SIG_BEGIN_INTERACTION, &(SysEvt_BeginInteraction){ entity });

  interactable = ecs_get(g_ecs, entity, INTERACTABLE);

  for (int i = 0; i < interactable->cnt; ++i)
  {
    items[i] = interactable->text[i];
  }

  ui_list_display((const char**)items, interactable->cnt);
  ui_list_set_pos(120, 120);
  ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_select));
}

void player_controller_system_update()
{

  ecs_entity_t player;
  Controller*  controller;

  update_pointed_entity();

  if ((player = get_player(g_ecs)) == ECS_NULL_ENT)
    return;

  controller = ecs_get(g_ecs, player, CONTROLLER);

  controller->desired_vel = VEC2_ZERO;

  if (key_pressed(KEY_UP))
  {
    controller->desired_vel.y -= 1 * 100;
  }
  if (key_pressed(KEY_DOWN))
  {
    controller->desired_vel.y += 1 * 100;
  }
  if (key_pressed(KEY_LEFT))
  {
    controller->desired_vel.x -= 1 * 100;
  }
  if (key_pressed(KEY_RIGHT))
  {
    controller->desired_vel.x += 1 * 100;
  }

  if (key_just_pressed(KEY_SELECT))
  {
    inventory_display();
    return;
  }

  if (g_curr_iteractable_entity == ECS_NULL_ENT)
  {
    if (key_just_pressed(KEY_A))
    {
      controller->action = CHARACTER_ACTION_REGULAR_ATK;
    }

    else if (key_just_pressed(KEY_B))
    {
      controller->action = CHARACTER_ACTION_SPECIAL_ATK;
    }
  }
  else
  {

    if (key_just_pressed(KEY_A))
      begin_interact(g_curr_iteractable_entity);
  }
}
