#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
#include "action.h"
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
  STAGGER,
  ENEMY_TAG,
  PLAYER_TAG,
  CAMERA_TARGET_TAG,
  ENABLE_TILE_COLLISION_TAG,
  CHARACTER_ANIMATOR_TAG,
  BOSS_ROOM,
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
  MANA,
  DOOR_ATTRIBUTES,
  HOLDER,
  ATTACK_MASK,
  REMOVE_IF_OFFSCREEN,
  ATTACKER,
  MOVE_SPEED,
  FACING_DIRECTION,
  ATTACK_TARGET,
  TRIGGER,
  SCRIPT,
  EMOJI,
  HAND_ANIMATION,
  STATS,
  AGILITY_CHANGED,
  VITALITY_CHANGED,
  INTELLIGENT_CHANGED,
  STRENGTH_CHANGED,
  DEFENSE,
  ATTACK_POWER,
  STATUS_EFFECT,
  UNABLE_TO_MOVE,
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

typedef struct Controller
{
  Vec2 desired_direction;
  u16  action;
  BOOL in_action;
} Controller;

typedef struct Hand
{
  ecs_entity_t weapon;
  Vec2         original_point;
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
  Callback                frame_callback;
  Callback                finished_callback;
  BOOL                    relative;
  float                   initial_length;
  Vec2                    initial_point;
  double                  initial_angle;
  int                     current_duration;
  int                     current_index;
} HandAnimation;

enum
{
  WPN_CAT_MEELLE,
  WPN_CAT_RANGED,
};
typedef struct WeaponAttributes
{
  s32 atk;
  u16 type_id;
  u16 category;
  u32 range;
} WeaponAttributes;

/*Weapon skills*/
typedef struct WeaponSwingAttack
{
  CharacterAction on_action;
  u32             wide;
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
  int  cooldown_timer;
  BOOL processing;
  u8   state[32];
} WeaponCast;

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
typedef struct DoorAttributes
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
  int duration;
} Stagger;

typedef struct
{
  int duration;
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
} ChestAttributes;

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
  Callback cb_finished;
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
  Callback cb_time_out;
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

void ladder_attrs_init(LadderAttributes* sw, const char* level, const char* dest, u16 direction);
void name_init(Name* name, const char* value);
void text_init(Text* text, const char* value, FONT* font, COLOR color);
void interactable_init(Interactable* i, const char* const cmds[]);

#endif // COMPONENTS_H
