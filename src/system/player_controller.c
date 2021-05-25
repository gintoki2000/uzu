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

extern Ecs* gEcs;
extern RECT gViewport;

#define INTERACTABLE_DISTANCE (TILE_SIZE * 1)
#define POINTER_DOWN_WIDTH 6
#define POINTER_DOWN_HEIGHT 3
#define MOVE_SPEED 100.f

ecs_entity_t gCurrentInteractingEntity = ECS_NULL_ENT;

//<--------------------------------event callbacks----------------------------------->//

static void on_list_select(pointer_t arg, const char* item);
static void on_level_unloaded(pointer_t arg, pointer_t event);

//<==================================================================================>//

static void on_list_select(SDL_UNUSED pointer_t arg, const char* item)
{
  ems_broadcast(MSG_COMANND_SELECTED,
                &(CommandSelectedMsg){
                    gCurrentInteractingEntity,
                    item,
                });
}

static void on_level_unloaded(SDL_UNUSED pointer_t arg, SDL_UNUSED pointer_t event)
{
  gCurrentInteractingEntity = ECS_NULL_ENT;
}

struct _QueryClosestInteracableEntityArgs
{
  Vec2         player_position;
  float        closest_distance;
  ecs_entity_t closest_entity;
};

static BOOL
query_closest_interactable_entity_callback(struct _QueryClosestInteracableEntityArgs* args,
                                           ecs_entity_t                               e)
{
  Vec2  entity_position = ett_get_position(gEcs, e);
  float dist            = vec2_dist(args->player_position, entity_position);
  if (dist < args->closest_distance && ecs_has(gEcs, e, INTERACTABLE))
  {
    args->closest_distance = dist;
    args->closest_entity   = e;
  }
  return TRUE;
}

static void update_interactable_entity()
{
  if (gCurrentInteractingEntity == ECS_NULL_ENT)
  {
    Vec2 player_pos;
    RECT rect;

    struct _QueryClosestInteracableEntityArgs cbargs;

    player_pos = snn_get_player_position(gEcs);
    rect.x     = player_pos.x - INTERACTABLE_DISTANCE / 2;
    rect.y     = player_pos.y - INTERACTABLE_DISTANCE / 2;
    rect.w     = INTERACTABLE_DISTANCE;
    rect.h     = INTERACTABLE_DISTANCE;

    cbargs.player_position  = player_pos;
    cbargs.closest_entity   = ECS_NULL_ENT;
    cbargs.closest_distance = 9999.f;

    collision_box_query(&rect,
                        0xffff,
                        CALLBACK_1(&cbargs, query_closest_interactable_entity_callback));
    gCurrentInteractingEntity = cbargs.closest_entity;
  }
  else
  {
    Vec2 player_pos, ientity_pos;

    player_pos  = snn_get_player_position(gEcs);
    ientity_pos = ett_get_position(gEcs, gCurrentInteractingEntity);

    if (vec2_dist(player_pos, ientity_pos) > INTERACTABLE_DISTANCE ||
        !ecs_has(gEcs, gCurrentInteractingEntity, INTERACTABLE))
    {
      gCurrentInteractingEntity = ECS_NULL_ENT;
    }
  }
}

static void begin_interact(ecs_entity_t entity)
{
  Interactable* interactable = ecs_get(gEcs, entity, INTERACTABLE);
  if (!interactable)
    return;
  ui_list_display((const char**)interactable->commands, interactable->numCommands);
  ui_list_set_pos(UI_LIST_POS_CENTER_X, UI_LIST_POS_CENTER_Y);
  ui_list_hook(UI_LIST_ON_SELECT, CALLBACK_2(on_list_select));
  ems_broadcast(MSG_BEGIN_INTERACTION, &(BeginInteractionMsg){ entity });
}

void player_process_input(SDL_UNUSED void* arg)
{

  ecs_entity_t      player;
  DesiredDirection* desired_direction;

  if ((player = scn_get_player(gEcs)) == ECS_NULL_ENT)
    return;
  if (ecs_has(gEcs, player, SCRIPT))
    return;
  update_interactable_entity();

  desired_direction = ecs_get(gEcs, player, DESIRED_DIRECTION);
  if (!ecs_has(gEcs, player, INVULNERABLE))
  {
    *desired_direction = VEC2_ZERO;
    if (button_pressed(BUTTON_UP))
      desired_direction->y -= 1.f;

    if (button_pressed(BUTTON_DOWN))
      desired_direction->y += 1.f;

    if (button_pressed(BUTTON_LEFT))
      desired_direction->x -= 1.f;

    if (button_pressed(BUTTON_RIGHT))
      desired_direction->x += 1.f;

    *desired_direction = vec2_unit(*desired_direction);

    if (button_just_pressed(BUTTON_OPEN_INVENTORY))
    {
      inventory_display();
      return;
    }
    if (mouse_button_just_pressed(SDL_BUTTON_LEFT) && !ecs_has(gEcs, player, ATTACK_COMMAND))
    {
      ecs_set(gEcs, player, ATTACK_COMMAND, &(AttackCommand){ .triggerCode = 0 });
    }

    if (button_just_pressed(BUTTON_JUMP))
    {
      Transform* transform = ecs_get(gEcs, player, TRANSFORM);
      if (transform->z <= 0.f)
      {
        Motion* motion = ecs_get(gEcs, player, MOTION);
        motion->vz     = 100.f;
        Mix_PlayChannel(-1, get_sfx(SFX_MOV_JUMP), 0);
      }
    }

    if (button_just_pressed(BUTTON_INTERACT) && gCurrentInteractingEntity != ECS_NULL_ENT)
      begin_interact(gCurrentInteractingEntity);
  }
}

void player_controller_system_init(void)
{
  ems_connect(MSG_LEVEL_UNLOADED, CALLBACK_2(on_level_unloaded));
}
