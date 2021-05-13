#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
#include "action.h"
#include "behavior_tree.h"
#include "global.h"
#include "path_finder.h"
#include "toolbox/toolbox.h"

typedef enum ComponentId
{
  AGGRO_AREA,
  AGILITY_CHANGED,
  ANIMATOR,
  ATTACKER,
  ATTACK_COMMAND,
  ATTACK_MASK,
  ATTACK_POWER,
  ATTACK_TARGET,
  ATTUNEMENT_SLOT,
  BLACKBOARD,
  BOSS_ROOM,
  BRAIN,
  CAMERA_TARGET_TAG,
  CHARACTER_ANIMATOR_TAG,
  CHEST,
  DEFENSE,
  DESIRED_DIRECTION,
  DESTROYED_TAG,
  DIALOGUE,
  DOOR_ATTRIBUTES,
  DOOR_TAG,
  DROP,
  EMOJI,
  ENABLE_TILE_COLLISION_TAG,
  ENEMY_TAG,
  AIM_DIRECTION,
  FOLLOWING_TARGET,
  HAND,
  HAND_ANIMATION,
  HEALTH,
  HEAL_BAR,
  HITBOX,
  HOLDER,
  INTELLIGENT_CHANGED,
  INTERACTABLE,
  INVULNERABLE,
  LADDER_ATTRIBUTES,
  LIFE_SPAN,
  MANA,
  MERCHANT,
  MOTION,
  MOVE_SPEED,
  NAME,
  PATH,
  PICKUPABLE_ATTRIBUTES,
  PLAYER_TAG,
  PROJECTILE_ATTRIBUTES,
  REMOVE_IF_OFFSCREEN,
  SCRIPT,
  STAGGER,
  STATS,
  STATUS_EFFECT,
  STRENGTH_CHANGED,
  TEXT,
  TRANSFORM,
  TRIGGER,
  UNABLE_TO_MOVE,
  VISUAL,
  VITALITY_CHANGED,
  WALK_DIRECTLY_TOWARD,
  WEAPON_ATTRIBUTES,
  WEAPON_CAST,
  WEAPON_CHARGE_ATTACK,
  WEAPON_SHOOT,
  WEAPON_SWING_ATTACK,
  WEAPON_THUNDER_STORM_RELEASE,
  WEAPON_THUST_ATTACK,
  INITIAL_POSITION,
  PATHFINDING_PARAMS,
  NUM_COMPONENTS
} ComponentId;

extern const EcsCompDesc gCompDescs[];

typedef struct Motion
{
  Vec2  vel;
  Vec2  acc;
  float friction;
  float vz;
  float gravity_scale;
  float bounching;
} Motion;

typedef struct Transform
{
  Vec2   lastPosition;
  Vec2   position;
  double rotation;
  float  z;
} Transform;

typedef struct AimDirection
{
  Vec2 value;
  BOOL frezze;
} AimDirection;

typedef struct Visual
{
  Sprite           sprite;
  SDL_Point        anchor;
  SDL_RendererFlip flip;
  COLOR            color;
} Visual;

typedef struct Spot
{
  Vec2  position;
  float radius;
} AggroArea;

typedef struct
{
  float value;
} DeaggroRadius;

typedef struct
{
  float value;
} AggroRadius;

typedef struct Hand
{
  ecs_entity_t weapon;
  Vec2         originalPoint;
  double       angle;
  float        length;
} Hand;

typedef struct HandAnimKeyFrame
{
  int    duration;
  double angle;
  Vec2   original;
  float  length;
} HandAnimKeyFrame;

typedef struct HandAnimation
{
  const HandAnimKeyFrame* keyframes;
  Callback                cbFrame;
  Callback                cbCompleted;
  BOOL                    relative;
  float                   initialLength;
  Vec2                    initialOriginalPoint;
  double                  initialAngle;
  int                     currentDuration;
  int                     currentIndex;
} HandAnimation;

enum
{
  WPN_CAT_MEELLE,
  WPN_CAT_RANGED,
};

typedef struct WeaponRange
{
  int min;
  int max;
} WeaponRange;

typedef struct WeaponAttributes
{
  s32         atk;
  u16         typeId;
  u16         category;
  WeaponRange range;
} WeaponAttributes;

/*Weapon skills*/
typedef struct WeaponSwingAttack
{
  u32 code;
  u32 wide;
} WeaponSwingAttack;

typedef struct WeaponChargeAttack
{
  u32 code;
} WeaponChargeAttack;

typedef struct WeaponThustAttack
{
  u32 code;
} WeaponThustAttack;

typedef struct WeaponThunderStormRelease
{
  u32 code;
} WeaponThunderStormRelease;

typedef struct WeaponShoot
{
  u32   code;
  u16   fire_rate;
  u16   timer;
  float projspd;
} WeaponShoot;

typedef struct WeaponCast
{
  u32  code;
  u32  cooldownTimer;
  BOOL processing;
  u8   state[32];
} WeaponCast;

typedef struct AttackCommand
{
  u32      code;
  BOOL     processing;
  Callback cbCompleted;
} AttackCommand;

typedef Vec2 DesiredDirection;

/*entity tags*/
typedef int PlayerTag;

typedef int EnemyTag;

typedef int CameraTargetTag;

typedef int EnableTileCollisionTag;

typedef int DestroyedTag;

typedef int CharacterAnimatorTag;

typedef struct BossRoom
{
  RECT rect;
} BossRoom;

typedef struct ProjectileAttributes
{
  int   damage;
  u16   effect;
  u16   damageType;
  BOOL  destroyWhenHit;
  BOOL  impact;
  Vec2  impactForce;
  float impactForceZ;
  u16   impactTime;
  u16   sfx;
  u16   shooter;
} ProjectileAttributes;

typedef enum DoorState
{
  DOOR_STATE_CLOSE,
  DOOR_STATE_OPEN,
} DoorState;

#define DOOR_NO_REQUIRED_KEY -1
#define DOOR_REQUIRED_SPECIAL_CONDITION -2
typedef struct DoorAttributes
{
  s16 requiredKey;
  u16 state;
} DoorAttributes;

typedef struct PickupableAttributes
{
  u16 id;
  u16 sfx;
  u8  quality;
} PickupableAttributes;

typedef struct
{
  u32              currentAnimation;
  u16              elapsed;
  const Animation* anims;
} Animator;

typedef struct
{
  Vec2 size;
  Vec2 anchor;
  int  proxyId;
  u16  mask;
  u16  category;
} HitBox;

typedef struct
{
  int       len; // in pixel
  SDL_Color color;
  SDL_Color border;
  SDL_Point anchor;
} HealthBar;

typedef struct LifeSpan
{
  s32 remaining;
} LifeSpan;

#define DROP_MAX_NUM_ITEMS 4
typedef struct Drop
{
  u16 type[DROP_MAX_NUM_ITEMS];
  u8  rate[DROP_MAX_NUM_ITEMS];
  u8  quality[DROP_MAX_NUM_ITEMS];
  u8  cnt;
} Drop;

//************^^status^^******************//
typedef struct
{
  int duration;
} Stagger;

typedef struct
{
  int duration;
} Invulnerable;
//<======================================>//

typedef struct
{
  BTRoot* root;
} Brain;

typedef struct WalkDirectlyToward
{
  Vec2     destination;
  Callback cbCompleted;
} WalkDirectlyToward;

typedef struct
{
  ecs_entity_t entity;
  float        radius;
} FollowingTarget;

#define LADDER_ATTRS_MAX_LEVEL_NAME_LEN 255
#define LADDER_ATTRS_MAX_DEST_LEN 255
typedef struct
{
  char level[LADDER_ATTRS_MAX_LEVEL_NAME_LEN + 1]; // level nào cần load
  char dest[LADDER_ATTRS_MAX_DEST_LEN + 1]; // vị trí cầu thang mà player sẽ xuất hiện
  Direction exitDirection;
} PortalAttributes;

#define NAME_MAX_LEN 15
typedef struct
{
  char value[NAME_MAX_LEN + 1];
} Name;

#define TEXT_MAX_LEN 63
typedef struct
{
  char      value[TEXT_MAX_LEN + 1];
  FC_Effect effect;
  FONT*     font;
} Text;

#define INTERACTABLE_MAX_COMMANDS 5
typedef struct Interatcable
{
  const char* commands[INTERACTABLE_MAX_COMMANDS];
  int         numCommands;
} Interactable;

typedef struct
{
  int conversation_id;
} Dialogue;

#define MERCHANT_MAX_PAYLOADS 100
#define MERCHANT_INIFINTE -1
typedef struct Merchant
{
  ItemPayload payloads[MERCHANT_MAX_PAYLOADS];
  u32         numPayloads;
} Merchant;

#define CHEST_MAX_ITEMS 5
enum
{
  CHEST_STATE_CLOSE,
  CHEST_STATE_OPEN,
};
typedef struct
{
  Item items[CHEST_MAX_ITEMS];
  u16  numSlots;
  u16  state;
  u32  id;
} ChestAttributes;

typedef struct
{
  u32 spellId;
} AttunementSlot;

typedef int RemoveIfOffScreen;

typedef struct Holder
{
  ecs_entity_t value;
} Holder;

typedef struct AttackMask
{
  u16 value;
} AttackMask;

typedef struct Attacker
{
  ecs_entity_t value;
} Attacker;

typedef struct AttackTarget
{
  ecs_entity_t value;
} AttackTarget;

typedef struct Stat
{
  u8 base;
  s8 modifier[3];
} Stat;

typedef struct Stats
{
  Stat vitality;
  Stat strength;
  Stat intelligent;
  Stat agility;
} Stats;

// events
typedef int AgilityChanged, VitalityChanged, IntelligentChanged, StrengthChanged;

typedef struct MoveSpeed
{
  int value;
} MoveSpeed;

typedef struct Mana
{
  u16 current;
  u16 max;
} Mana;

typedef struct Health
{
  u16 current;
  u16 max;
} Health;

typedef struct Defense
{
  u8 physical;
  u8 fire;
  u8 ice;
  u8 lightning;
} Defense;

typedef struct
{
  u8 physical;
  u8 magical;
} AttackPower;

typedef struct Script
{
  Action*  action;
  Callback cbFinished;
} Script;

enum EmojiType
{
  EMOJI_QUESTION,
  EMOJI_BANG,
  EMOJI_SILENT,
  EMOJI_HEART,
  NUM_EMOJIES,
};

typedef struct Emoji
{
  int      id;
  int      duration;
  Callback cbTimeOut;
} Emoji;

typedef struct StatusEffect
{
  u16 type;
  u16 duration;
  u16 elapsed;
  u8  state[16];
} StatusEffect;

typedef struct UnableToMove
{
  u16 count;
} UnableToMove;

typedef Vec2 InitialPosition;

#define PATH_MAX_NUM_NODES 100
typedef struct Path
{
  POINT    nodes[100];
  int      count;
  int      currentIndex;
  Callback cbCompleted;
} Path;

typedef struct PathfindingParams
{
  POINT    start;
  POINT    goal;
  Callback cbCompleted;
} PathfindingParams;

void portal_attrs_init(PortalAttributes* sw, const char* level, const char* dest, u16 direction);
void name_init(Name* name, const char* value);
void text_init(Text* text, const char* value, FONT* font, COLOR color);
void interactable_init(Interactable* i, const char* const cmds[]);

#endif // COMPONENTS_H
