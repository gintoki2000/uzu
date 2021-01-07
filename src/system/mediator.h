#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  SYS_SIG_ENTITY_DIED, // entity hết máu
  SYS_SIG_WEAPON_HIT, //vũ khí chạm vào entity
  SYS_SIG_COLLISION, // va chạm giữa hai entity
  SYS_SIG_PROJECTILE_HIT,
  SYS_SIG_LIFE_SPAN_FINISHED, //kết thúc thời gian sống
  SYS_SIG_DEAL_DAMAGE, // gây ra sát thương
  SYS_SIG_GET_DAMAGED, // entity nhận sát thương
  SYS_SIG_PLAYER_HIT_ITEM, // player chạm item
  SYS_SIG_HIT_TRAP, // player chạm phải bẩy
  SYS_SIG_HIT_LADDER, // player chạm vào cầu thang (chuyển map)
  SYS_SIG_END_INTERACTION, // kết thúc tương tác với interactable entity
  SYS_SIG_BEGIN_INTERACTION, // bắt đầu tương tác với interactable entity
  SYS_SIG_CONVERSATION_FINISHED, // một đoạn hội thoại đã hoàng thành
  SYS_SIG_COMANND_SELECTED, // player chọn một cmd các system lắng nghe và thực thi
  SYS_SIG_EVENT_FINISHED, // một sự kiện(cốt truyện) trong game hoàn thành
  SYS_SIG_ITEM_USED, // item đã được sử dụng
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
} SysEvt_LifeSpanFinished;

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

typedef struct
{
  ecs_entity_t entity;
} SysEvt_BeginInteraction;

typedef struct
{
  ecs_entity_t entity;
} SysEvt_EndInteraction;

typedef struct
{
  ecs_entity_t entity;
  const char*  cmd;
} SysEvt_CommandSelected;

typedef struct
{
  ecs_entity_t npc;
  const char*  npc_name;
  const char*  conversation_name;
  const char*  response;
} SysEvt_ConversationFinished;

typedef struct
{
  int event_code;
} SysEvt_EventFinished;

void mediator_init();
void mediator_fini();
void mediator_connect(int sig_id, pointer_t arg, slot_t slot);
void mediator_disconnect(int sig_id, pointer_t func_or_instance);
void mediator_broadcast(int sig_id, const pointer_t event);

#endif // MEDIATOR_H
