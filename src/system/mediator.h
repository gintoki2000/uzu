#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  SYS_SIG_ENTITY_DIED,
  SYS_SIG_WEAPON_HIT,
  SYS_SIG_COLLISION,
  SYS_SIG_PROJECTILE_HIT,
  SYS_SIG_FINISH_LIFE_SPAN,
  SYS_SIG_DEAL_DAMAGE,
  SYS_SIG_GET_DAMAGED,
  SYS_SIG_PLAYER_HIT_ITEM,
  SYS_SIG_HIT_TRAP,
  SYS_SIG_HIT_LADDER,
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
  int          type;
} SysEvt_DealDamage;

typedef struct
{
  ecs_entity_t entity;
} SysEvt_FinishLifeSpan;

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
  int          type;
} SysEvt_GetDamaged;

typedef struct
{
  ecs_entity_t player;
  ecs_entity_t item;
} SysEvt_PlayerHitItem;

typedef struct
{
  int          row, col;
  ecs_entity_t entity;
} SysEvt_EntityHitTrap;

typedef struct
{
  ecs_entity_t ladder;
} SysEvt_HitLadder;

void mediator_init();
void mediator_fini();
void mediator_connect(int sig_id, pointer_t arg, slot_t slot);
void mediator_disconnect(pointer_t func);
void mediator_broadcast(int sig_id, const pointer_t event);

#endif // MEDIATOR_H
