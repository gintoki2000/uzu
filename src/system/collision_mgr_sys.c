#ifndef COLLISION_FILTER_H
#define COLLISION_FILTER_H
#include "collision_mgr_sys.h"

#include "mediator.h"
#include <components.h>
#include <toolbox/toolbox.h>

extern Ecs* g_ecs;

static void on_collision(void* arg, SysEvt_Collision* event);
static void weapon_vs_entity(ecs_entity_t weapon, ecs_entity_t entity);
static void entity_vs_weapon(ecs_entity_t entity, ecs_entity_t weapon);
static void projectile_vs_entity(ecs_entity_t projectile, ecs_entity_t entity);
static void entity_vs_projectile(ecs_entity_t entity, ecs_entity_t projectile);
static void player_vs_item(ecs_entity_t player, ecs_entity_t item);
static void item_vs_player(ecs_entity_t item, ecs_entity_t player);

static void (*_table[NUM_CATEGORIES][NUM_CATEGORIES])(ecs_entity_t, ecs_entity_t) =
{
  [CATEGORY_PLAYER] = {
    [CATEGORY_WEAPON] = entity_vs_weapon,
    [CATEGORY_PROJECTILE] = entity_vs_projectile,
    [CATEGORY_ITEM] = player_vs_item, 
  },
  [CATEGORY_ENEMY] = {
    [CATEGORY_WEAPON ] = entity_vs_weapon,
    [CATEGORY_PROJECTILE] = entity_vs_projectile,
  },
  [CATEGORY_PROJECTILE] = {
    [CATEGORY_ENEMY] = projectile_vs_entity,
    [CATEGORY_PLAYER] = projectile_vs_entity,
  },
  [CATEGORY_WEAPON] = {
    [CATEGORY_ENEMY] = weapon_vs_entity,
    [CATEGORY_PLAYER] = weapon_vs_entity,
  },
  [CATEGORY_ITEM] = {
    [CATEGORY_PLAYER] = item_vs_player,
  }
};

void collision_manager_system_init()
{
  mediator_connect(SYS_SIG_COLLISION, NULL, SLOT(on_collision));
}

static void on_collision(void* arg, SysEvt_Collision* event)
{
  HitBox* hitbox1;
  HitBox* hitbox2;
  void (*fn)(ecs_entity_t, ecs_entity_t);

  if ((hitbox1 = ecs_get(g_ecs, event->e1, HITBOX)) != NULL &&
      (hitbox2 = ecs_get(g_ecs, event->e2, HITBOX)) != NULL)
  {

    fn = _table[hitbox1->category][hitbox2->category];
    if (fn != NULL)
      fn(event->e1, event->e2);
  }
}

static void weapon_vs_entity(ecs_entity_t weapon, ecs_entity_t entity)
{
  mediator_broadcast(SYS_SIG_WEAPON_HIT,
                &(SysEvt_WeaponHit){
                    .weapon = weapon,
                    .entity = entity,
                });
}
static void entity_vs_weapon(ecs_entity_t entity, ecs_entity_t weapon)
{
  mediator_broadcast(SYS_SIG_WEAPON_HIT,
                &(SysEvt_WeaponHit){
                    .weapon = weapon,
                    .entity = entity,
                });
}
static void projectile_vs_entity(ecs_entity_t projectile, ecs_entity_t entity)
{
  mediator_broadcast(SYS_SIG_PROJECTILE_HIT,
                &(SysEvt_ProjectileHit){
                    .projectile = projectile,
                    .entity     = entity,
                });
}
static void entity_vs_projectile(ecs_entity_t entity, ecs_entity_t projectile)
{
  mediator_broadcast(SYS_SIG_PROJECTILE_HIT,
                &(SysEvt_ProjectileHit){
                    .projectile = projectile,
                    .entity     = entity,
                });
}

static void player_vs_item(ecs_entity_t player, ecs_entity_t item)
{
  mediator_broadcast(SYS_SIG_PLAYER_HIT_ITEM,
                &(SysEvt_PlayerHitItem){
                    .item   = item,
                    .player = player,
                });
}

static void item_vs_player(ecs_entity_t item, ecs_entity_t player)
{
  mediator_broadcast(SYS_SIG_PLAYER_HIT_ITEM,
                &(SysEvt_PlayerHitItem){
                    .item   = item,
                    .player = player,
                });
}

#endif // COLLISION_FILTER_H
