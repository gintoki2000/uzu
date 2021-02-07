#ifndef COLLISION_FILTER_H
#define COLLISION_FILTER_H
#include "system/game_logic.h"

#include "system/event_messaging_sys.h"
#include "components.h"
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

static void on_collision(void* arg, MSG_Collision* event);
static void weapon_vs_entity(ecs_entity_t weapon, ecs_entity_t entity);
static void entity_vs_weapon(ecs_entity_t entity, ecs_entity_t weapon);
static void projectile_vs_entity(ecs_entity_t projectile, ecs_entity_t entity);
static void entity_vs_projectile(ecs_entity_t entity, ecs_entity_t projectile);
static void player_vs_item(ecs_entity_t player, ecs_entity_t item);
static void item_vs_player(ecs_entity_t item, ecs_entity_t player);
static void player_vs_ladder(ecs_entity_t player, ecs_entity_t ladder);
static void ladder_vs_player(ecs_entity_t ladder, ecs_entity_t player);
static void interacable_vs_entity(ecs_entity_t interacable, ecs_entity_t entity);
static void entity_vs_interacable(ecs_entity_t entity, ecs_entity_t interacable);
static void entity_vs_trigger(ecs_entity_t entity, ecs_entity_t trigger);
static void trigger_vs_entity(ecs_entity_t trigger, ecs_entity_t entity);

static void (*s_handler_fn_tbl[NUM_CATEGORIES][NUM_CATEGORIES])(ecs_entity_t, ecs_entity_t) =
{
  [CATEGORY_PLAYER] = {
    [CATEGORY_WEAPON] = entity_vs_weapon,
    [CATEGORY_PROJECTILE] = entity_vs_projectile,
    [CATEGORY_ITEM] = player_vs_item, 
    [CATEGORY_LADDER] = player_vs_ladder,
    [CATEGORY_INTERACABLE] = entity_vs_interacable,
    [CATEGORY_TRIGGER] = entity_vs_trigger
  },
  [CATEGORY_ENEMY] = {
    [CATEGORY_WEAPON ] = entity_vs_weapon,
    [CATEGORY_PROJECTILE] = entity_vs_projectile,
  },
  [CATEGORY_PROJECTILE] = {
    [CATEGORY_ENEMY] = projectile_vs_entity,
    [CATEGORY_PLAYER] = projectile_vs_entity,
    [CATEGORY_INTERACABLE] = projectile_vs_entity
  },
  [CATEGORY_WEAPON] = {
    [CATEGORY_ENEMY] = weapon_vs_entity,
    [CATEGORY_PLAYER] = weapon_vs_entity,
  },
  [CATEGORY_ITEM] = {
    [CATEGORY_PLAYER] = item_vs_player,
  },
  [CATEGORY_LADDER] = {
    [CATEGORY_PLAYER] = ladder_vs_player,
  },
  [CATEGORY_INTERACABLE] = {
    [CATEGORY_ENEMY] = interacable_vs_entity,
    [CATEGORY_PLAYER] = interacable_vs_entity,
    [CATEGORY_PROJECTILE] = interacable_vs_entity,
  },
  [CATEGORY_TRIGGER] = {
    [CATEGORY_PLAYER] = trigger_vs_entity,
  },
};

void collision_manager_system_init()
{
  ems_connect(MSG_COLLISION, NULL, on_collision);
}

static void on_collision(SDL_UNUSED void* arg, MSG_Collision* event)
{
  HitBox* hitbox1;
  HitBox* hitbox2;
  void (*fn)(ecs_entity_t, ecs_entity_t);

  if ((hitbox1 = ecs_get(g_ecs, event->e1, HITBOX)) != NULL &&
      (hitbox2 = ecs_get(g_ecs, event->e2, HITBOX)) != NULL)
  {

    fn = s_handler_fn_tbl[hitbox1->category][hitbox2->category];
    if (fn != NULL)
      fn(event->e1, event->e2);
  }
}

static void weapon_vs_entity(ecs_entity_t weapon, ecs_entity_t entity)
{
  ems_broadcast(MSG_WEAPON_HIT,
                &(MSG_WeaponHit){
                    .weapon = weapon,
                    .entity = entity,
                });
}
static void entity_vs_weapon(ecs_entity_t entity, ecs_entity_t weapon)
{
  ems_broadcast(MSG_WEAPON_HIT,
                &(MSG_WeaponHit){
                    .weapon = weapon,
                    .entity = entity,
                });
}
static void projectile_vs_entity(ecs_entity_t projectile, ecs_entity_t entity)
{
  ems_broadcast(MSG_PROJECTILE_HIT,
                &(MSG_ProjectileHit){
                    .projectile = projectile,
                    .entity     = entity,
                });
}
static void entity_vs_projectile(ecs_entity_t entity, ecs_entity_t projectile)
{
  ems_broadcast(MSG_PROJECTILE_HIT,
                &(MSG_ProjectileHit){
                    .projectile = projectile,
                    .entity     = entity,
                });
}

static void player_vs_item(ecs_entity_t player, ecs_entity_t item)
{
  ems_broadcast(MSG_PLAYER_HIT_ITEM,
                &(MSG_HitPickupableEntity){
                    .pickupable_entity   = item,
                    .player = player,
                });
}

static void item_vs_player(ecs_entity_t item, ecs_entity_t player)
{
  ems_broadcast(MSG_PLAYER_HIT_ITEM,
                &(MSG_HitPickupableEntity){
                    .pickupable_entity   = item,
                    .player = player,
                });
}

static void player_vs_ladder(ecs_entity_t player, ecs_entity_t ladder)
{
  (void)player;
  ems_broadcast(MSG_HIT_LADDER,
                &(MSG_HitLadder){
                    .ladder = ladder,
                });
}
static void ladder_vs_player(ecs_entity_t ladder, ecs_entity_t player)
{
  (void)player;
  ems_broadcast(MSG_HIT_LADDER,
                &(MSG_HitLadder){
                    .ladder = ladder,
                });
}

static void interacable_vs_entity(ecs_entity_t interacable, ecs_entity_t entity)
{
  if (ecs_has(g_ecs, interacable, DOOR_ATTRIBUTES))
  {
    ems_broadcast(MSG_HIT_DOOR, &(MSG_HitDoor){ .door = interacable, .entity = entity });
  }
}

static void entity_vs_interacable(ecs_entity_t entity, ecs_entity_t interacable)
{
  interacable_vs_entity(interacable, entity);
}

static void entity_vs_trigger(ecs_entity_t entity, ecs_entity_t trigger)
{
  ems_broadcast(MSG_HIT_TRIGGER, &(MSG_HitTrigger){ .entity = entity, .trigger = trigger });
}

static void trigger_vs_entity(ecs_entity_t trigger, ecs_entity_t entity)
{
  ems_broadcast(MSG_HIT_TRIGGER, &(MSG_HitTrigger){ .entity = entity, .trigger = trigger });
}
#endif // COLLISION_FILTER_H
