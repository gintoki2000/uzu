#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
#include "behaviour_tree.h"
#include "global.h"
#include "path_finder.h"
#include "toolbox/toolbox.h"

typedef enum ComponentId
{
  TRANSFORM,
  VISUAL,
  MOTION,
  ANIMATOR,
  CONTROLLER,
  HAND,
  HEALTH,
  HITBOX,
  HEAL_BAR,
  LIFE_SPAN,
  DESTROYED_TAG,
  WEAPON_SWING_ATTACK,
  WEAPON_CHARGE_ATTACK,
  WEAPON_THUST_ATTACK,
  WEAPON_THUNDER_STORM_RELEASE,
  WEAPON_SHOOT,
  WEAPON_ATTRIBUTES,
  DROP,
  PICKUPABLE_ATTRIBUTES,
  INVULNERABLE,
  PARALYZED,
  ENEMY_TAG,
  PLAYER_TAG,
  CAMERA_TARGET_TAG,
  ENABLE_TILE_COLLISION_TAG,
  CHARACTER_ANIMATOR_TAG,
  PROJECTILE_ATTRIBUTES,
  DOOR_TAG,
  BRAIN,
  DESTINATION,
  PATH,
  FOLLOWING_TARGET,
  AGGRO_AREA,
  LADDER_ATTRIBUTES,
  NAME,
  INTERACTABLE,
  TEXT,
  DIALOGUE,
  MERCHANT,
  CHEST,
  ATTUNEMENT_SLOT,
  WEAPON_CAST,
  MANA_POOL,
  DOOR_ATTRIBUTES,
  HOLDER,
  ATTACK_MASK,
  REMOVE_IF_OFFSCREEN,
  ATTACKER,
  SELF_DESTRUCTION,
  MOVE_SPEED,
  FACING_DIRECTION,
  ATTACK_TARGET,
  NUM_COMPONENTS
} ComponentId;

typedef struct Motion
{
  Vec2  vel;
  Vec2  acc;
  float max_force;
  float friction;
  float vz;
  float gravity_scale;
  float bounching;
} Motion;

typedef struct Transform
{
  Vec2   prev_position;
  Vec2   position;
  double rotation;
  float  z;
} Transform;

typedef struct FacingDirection
{
  Vec2 value;
  BOOL frezze;
} FacingDirection;

typedef struct Visual
{
  SDL_Point        anchor;
  SDL_RendererFlip flip;
  Sprite           sprite;
  COLOR            color;
} Visual;

typedef struct Spot
{
  Vec2  position;
  float radius;
} AggroArea;

typedef struct Controller
{
  Vec2 desired_direction;
  u16  action;
  BOOL in_action;
} Controller;

typedef struct Equipment
{
  ecs_entity_t weapon;
  Vec2         attach_point;
  Vec2         original_point;
  Vec2         adjustment;
} Hand;

typedef struct WeaponAttributes
{
  s32  atk;
  u16  type_id;
  BOOL rotate_hand;
} WeaponAttributes;

/*Weapon skills*/
typedef struct WeaponSwingAttack
{
  CharacterAction on_action;
  u16             timer;
  u16             state;
  u32             range;
} WeaponSwingAttack;

typedef struct WeaponChargeAttack
{
  u16  on_action;
  u16  timer;
  BOOL is_active;
} WeaponChargeAttack;

typedef struct WeaponThustAttack
{
  u16 on_action;
  u16 timer;
  u8  state;
} WeaponThustAttack;

typedef struct WeaponThunderStormRelease
{
  u16 on_action;
  u16 remaining;
  u16 interval;
  u16 total;
  u16 timer;
} WeaponThunderStormRelease;

typedef struct WeaponShoot
{
  u16   on_action;
  u16   fire_rate;
  u16   timer;
  float projspd;
} WeaponShoot;

typedef struct WeaponCast
{
  int timer;
} Castable;

/*entity tags*/
typedef int PlayerTag;

typedef int EnemyTag;

typedef int CameraTargetTag;

typedef int EnableTileCollisionTag;

typedef int DestroyedTag;

typedef int CharacterAnimatorTag;

typedef struct ProjectileAttributes
{
  int   damage;
  u16   effect;
  u16   damage_type;
  BOOL  destroy_when_hit;
  BOOL  impact;
  Vec2  impact_force;
  float impact_force_z;
  u16   impact_time;
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
typedef struct DoorInfo
{
  s16 required_key;
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
  u32              current_anim;
  u16              elapsed;
  const Animation* anims;
} Animator;

typedef struct
{
  Vec2 size;
  Vec2 anchor;
  int  proxy_id;
  u16  mask_bits;
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
  int remaining;
} Paralyzed;

typedef struct
{
  int remaining;
} Invulnerable;
//<======================================>//

typedef struct
{
  BTNode* root;
} Brain;

typedef Vec2 Destination;

#define PATH_MAX_NUM_NODES 100
typedef struct Path
{
  Vec2i nodes[PATH_MAX_NUM_NODES];
  int   num_nodes;
  int   curr;
} Path;

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
  Direction exit_direction;
} LadderAttributes;

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
  int         num_commands;
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
  u32         num_payloads;
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
  u16  num_slots;
  u16  state;
  u32  id;
} Chest;

typedef struct
{
  u16 spell_id;
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

// dùng để track ai tấn công entity
typedef struct Attacker
{
  ecs_entity_t value;
} Attacker;

typedef struct AttackTarget
{
  ecs_entity_t value;
} AttackTarget;

typedef struct SelfDestruction
{
  u32          range;
  ecs_entity_t target;
} SelfDestruction;

typedef struct Stat
{
  int base;
  int modifier[3]; // self-buff/weapon/hmm
} Stat;

typedef struct Stats
{
  Stat vitality;    // health
  Stat strenght;    // physical damage
  Stat intelligent; // mp & magic damage
} Stats;

// Combat attributes
typedef struct MoveSpeed
{
  int value;
} MoveSpeed;

typedef struct ManaPool
{
  u16 mana_points;
  u16 max_mana_points;
} ManaPool;

typedef struct Health
{
  s32 hit_points;
  s32 max_hit_points;
} Health;

typedef struct Defense
{
  u16 physical;
  u16 magical;
  u16 fire;
  u16 ice;
  u16 lightning;
} Defense;

void ladder_attrs_init(LadderAttributes* sw, const char* level, const char* dest, u16 direction);
void name_init(Name* name, const char* value);
void text_init(Text* text, const char* value, FONT* font, COLOR color);

#endif // COMPONENTS_H
