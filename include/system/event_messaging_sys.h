#ifndef MEDIATOR_H
#define MEDIATOR_H
#include "ecs/ecs.h"
#include "rfl.h"
#include "toolbox/toolbox.h"

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
  MSG_LEVEL_UNLOADED,
  MSG_GAME_SCENE_UNLOAD,
  MSG_GAME_SCENE_LOADED,
  MSG_HIT_DOOR,
  MSG_HIT_TRIGGER,
  MSG_ITEM_PICKED_UP,
  MSG_COIN_PICKED_UP,
  MSG_GAME_PAUSED,
  MSG_GAME_RESUMED,
  NUM_MSGS,
};

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} OverlapMsg;

typedef struct
{
  ecs_entity_t entity;
} EntityDiedMsg;


typedef struct
{
  ecs_entity_t weapon;
  ecs_entity_t entity;
} WeaponHitMsg;

typedef struct
{
  ecs_entity_t projectile;
  ecs_entity_t entity;
} ProjectileHitMsg;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t receiver;
  int          damage;
  int          type;
  u16          impactTime;
  Vec2         force;
  float        zforce;
} InflictDamageMsg;

typedef struct
{
  ecs_entity_t entity;
} LifeSpanFinishedMsg;

typedef struct
{
  ecs_entity_t entity;
  s32          row;
  s32          col;
} TileCollisionMsg;

typedef struct
{
  ecs_entity_t dealer;
  ecs_entity_t damagee;
  int          damage;
  int          type;
} GetDamagedMsg;

typedef struct MSG_HitPickupableEntity
{
  ecs_entity_t player;
  ecs_entity_t pickupableEntity;
} HitPickupableEntityMsg;

typedef struct
{
  int          row, col;
  ecs_entity_t entity;
} EntityHitTrapMsg;

typedef struct
{
  ecs_entity_t portal;
} EnterPortalMsg;

typedef struct
{
  ecs_entity_t entity;
} BeginInteractionMsg;

typedef struct
{
  ecs_entity_t entity;
} EndInteractionMsg;


typedef struct
{
  ecs_entity_t entity;
  const char*  cmd;
} CommandSelectedMsg;



typedef struct
{
  ecs_entity_t npc;
  const char*  response;
  u16          id;
  const char*  name;
} ConversationFinishedMsg;


typedef struct
{
  const char* event;
  const char* status;
} EventFinishedMsg;

typedef struct
{
  const char* level_name;
} LevelLoadedMsg;

typedef struct
{
  const char* level_name;
} LevelUnloadedMsg;

typedef struct
{
  ecs_entity_t entity;
  ecs_entity_t door;
} HitDoorMsg;

typedef struct

{
  ecs_entity_t entity;
  ecs_entity_t trigger;
} HitTriggerMsg;

typedef struct
{
  ecs_entity_t pickupable_entity;
  u16          itemTypeId;
  Vec2         position;
} ItemPickedUpMsg;

typedef struct
{
  u16  amount;
  Vec2 position;
} CoinPickedUpMsg;

void ems_init(void);
void ems_fini(void);
void ems_connect(int signal, Callback callback);
void ems_disconnect(int signal, Func fn);
void ems_disconnect_ex(int signal, void* instance, Func fn);
void ems_broadcast(int signal, const void* data);

#endif // MEDIATOR_H
