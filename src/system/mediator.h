#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  SIG_ENTITY_DIED,
  SIG_WEAPON_HIT,
  SIG_COLLISION,
  SIG_PROJECTILE_HIT,
  SIG_LIFE_SPAN_END_UP,
  SIG_DEAL_DAMAGE,
  SIG_GET_DAMAGED,
  SIG_PLAYER_HIT_ITEM,
  NUM_SYSTEM_SIGNALS,
};

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} SysEvt_Collision;

typedef struct
{
  ecs_entity_t entity;
} SysEvt_EntityDied;

typedef struct
{
  ecs_entity_t weapon;
  ecs_entity_t entity;
} SysEvt_WeaponHit;

typedef struct
{
  ecs_entity_t projectile;
  ecs_entity_t entity;
} SysEvt_ProjectileHit;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t receiver;
  int          damage;
} SysEvt_DealDamage;

typedef struct
{
  ecs_entity_t entity;
} SysEvt_LifeSpanEndUp;

typedef struct
{
  ecs_entity_t entity;
  s32          row;
  s32          col;
} SysEvt_TileCollision;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t damagee;
  int          damage;
} SysEvt_GetDamaged;

typedef struct 
{
  ecs_entity_t player;
  ecs_entity_t item;
} SysEvt_PlayerHitItem;

void mediator_init();
void mediator_fini();
void mediator_connect(int sig_id, pointer_t udata, slot_t slot);
void mediator_disconnect(pointer_t func);
void mediator_emit(int sig_id, const pointer_t event);

#endif // MEDIATOR_H
