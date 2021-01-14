#ifndef MEDIATOR_H
#define MEDIATOR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  MSG_ENTITY_DIED, // entity hết máu
  MSG_WEAPON_HIT,  // vũ khí chạm vào entity
  MSG_COLLISION,   // va chạm giữa hai entity
  MSG_PROJECTILE_HIT,
  MSG_LIFE_SPAN_FINISHED,    // kết thúc thời gian sống
  MSG_DEAL_DAMAGE,           // gây ra sát thương
  MSG_GET_DAMAGED,           // entity nhận sát thương
  MSG_PLAYER_HIT_ITEM,       // player chạm item
  MSG_HIT_TRAP,              // player chạm phải bẩy
  MSG_HIT_LADDER,            // player chạm vào cầu thang (chuyển map)
  MSG_END_INTERACTION,       // kết thúc tương tác với interactable entity
  MSG_BEGIN_INTERACTION,     // bắt đầu tương tác với interactable entity
  MSG_CONVERSATION_FINISHED, // một đoạn hội thoại đã hoàng thành
  MSG_COMANND_SELECTED,      // player chọn một cmd các system lắng nghe và thực thi
  MSG_EVENT_FINISHED,        // một sự kiện(cốt truyện) trong game hoàn thành
  MSG_ITEM_USED,             // item đã được sử dụng
  MSG_NEW_GAME,
  MSG_LEVEL_LOADED,
  MSG_GAME_SCENE_UNLOAD,
  MSG_GAME_SCENE_LOADED,
  NUM_MSGS,
};

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} MSG_Collision;

typedef struct
{
  ecs_entity_t entity;
} MSG_EntityDied;

typedef struct
{
  ecs_entity_t weapon;
  ecs_entity_t entity;
} MSG_WeaponHit;

typedef struct
{
  ecs_entity_t projectile;
  ecs_entity_t entity;
} MSG_ProjectileHit;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t receiver;
  int          damage;
  int          type;
} MSG_DealDamage;

typedef struct
{
  ecs_entity_t entity;
} MSG_LifeSpanFinished;

typedef struct
{
  ecs_entity_t entity;
  s32          row;
  s32          col;
} MSG_TileCollision;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t damagee;
  int          damage;
  int          type;
} MSG_GetDamaged;

typedef struct
{
  ecs_entity_t player;
  ecs_entity_t item;
} MSG_PlayerHitItem;

typedef struct
{
  int          row, col;
  ecs_entity_t entity;
} MSG_EntityHitTrap;

typedef struct
{
  ecs_entity_t ladder;
} MSG_HitLadder;

typedef struct
{
  ecs_entity_t entity;
} MSG_BeginInteraction;

typedef struct
{
  ecs_entity_t entity;
} MSG_EndInteraction;

typedef struct
{
  ecs_entity_t entity;
  const char*  cmd;
} MSG_CommandSelected;

typedef struct
{
  ecs_entity_t npc;
  const char*  npc_name;
  const char*  conversation_name;
  const char*  response;
} MSG_ConversationFinished;

typedef struct
{
  int event_code;
} MSG_EventFinished;

typedef struct
{
  const char* level_name;
} MSG_LevelLoaded;

void ems_init();
void ems_fini();
void ems_connect(int sig_id, pointer_t arg, funcptr_t func);
void ems_disconnect(int sig_id, pointer_t func_or_instance);
void ems_broadcast(int sig_id, const pointer_t event);

#endif // MEDIATOR_H
