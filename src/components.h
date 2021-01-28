#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
#include "behaviour_tree.h"
#include "path_finder.h"
#include <toolbox/toolbox.h>
#include <types.h>

typedef enum
{
  TRANSFORM,
  VISUAL,
  MOTION,
  ANIMATOR,
  CONTROLLER,
  EQUIPMENT,
  HEALTH,
  HITBOX,
  HEAL_BAR,
  LIFE_SPAN,
  TAG_TO_BE_DESTROYED,
  WEAPON_SKILL_SWING,
  WEAPON_SKILL_CHARGE,
  WEAPON_SKILL_THUST,
  WEAPON_SKILL_THUNDER_STORM,
  DAMAGE_OUTPUT,
  WEAPON_ATTRIBUTES,
  DROP,
  ITEM_TAG,
  INVULNERABLE,
  INPUT_BLOCKER,
  ENEMY_TAG,
  PLAYER_TAG,
  CAMERA_TARGET_TAG,
  TILE_COLLISION_TAG,
  CHARACTER_ANIMATOR_TAG,
  DOOR_TAG,
  AI_AGENT,
  DESTINATION,
  PATH,
  FOLLOWING_TARGET,
  SPOT,
  LEVEL_SWITCHER,
  NAME,
  INTERACTABLE,
  TEXT,
  DIALOGUE,
  MERCHANT,
  CHEST,
  ATTUNEMENT_SLOT,
  CASTABLE,
  MANA_POOL,
  DOOR_INFO,
  NUM_COMPONENTS
} ComponentId;

typedef struct
{
  Vec2  vel;
  Vec2  acc;
  float max_speed;
  float max_force;
  float friction;
} Motion;

typedef struct
{
  Vec2   prev_pos;
  Vec2   pos;
  double prev_rot;
  double rot;
} Transform;

typedef struct
{
  SDL_Point        anchor;
  SDL_RendererFlip flip;
  Sprite           sprite;
  COLOR            color;
  u8               opacity;
} Visual;

typedef struct
{
  Vec2  pos;
  float radius;
} Spot;

typedef struct
{
  Vec2 force;
  u16  action;
  BOOL in_action;
} Controller;

typedef struct
{
  ecs_entity_t weapon;
  Vec2         weapon_anchor;
  Vec2         d;
} Equipment;

typedef struct
{
  ecs_entity_t wearer;
  s32          atk;
  u16          type_id;
  u16          mask;
} WeaponAttributes;

/*Weapon skills*/
typedef struct
{
  CharacterAction on_action;
  int             timer;
  int             step;
  BOOL            is_active;
} wpskl_Swing;

typedef struct
{
  u16  on_action;
  u16  timer;
  BOOL is_active;
} wpskl_Charge;

typedef struct
{
  u16 on_action;
  u16 timer;
  u8  state;
} wpskl_Thust;

typedef struct
{
  u16 on_action;
  u16 remaining;
  u16 interval;
  u16 total;
  u16 timer;
} wpskl_ThunderStorm;

typedef struct
{
  int  atk;
  int  type;
  BOOL impact;
  Vec2 force;
} DamageOutput;

typedef struct 
{
  int w;
  int h;
  int cx;
  int cy;
} DamageArea;

/*entity tag*/
typedef int PlayerTag;

typedef int EnemyTag;

typedef int WeaponTag;

typedef int CameraTargetTag;

typedef int TileCollisionTag;

typedef int ProjectileTag;

typedef int NpcTag;

typedef int TagToBeDestroyed;

typedef int CharacterAnimatorTag;

typedef enum DoorState
{
  DOOR_STATE_CLOSE,
  DOOR_STATE_OPEN,
} DoorState;

typedef struct DoorInfo
{
  u16 required_key;
  u16 state;
} DoorInfo;

typedef struct
{
  ItemTypeId item_id;
} ItemTag;

typedef struct
{
  s32 hit_points;
  s32 max_hit_points;
} Health;

typedef struct
{
  u32        current_anim;
  u32        elapsed;
  u32        cnt;
  Animation* anims;
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

typedef struct
{
  ItemTypeId item1;
  ItemTypeId item2;
  int        change1;
  int        change2;
} Drop;

typedef struct
{
  int remaining;
} InputBlocker;

typedef struct
{
  int remaining;
} Invulnerable;

typedef ecs_entity_t Attacker;

typedef struct
{
  BTNode* root;
} AIAgent;

//** BT Vars

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

typedef struct
{
  char*     level;
  char*     dest;
  Direction exit_direction;
} LevelSwitcher;

typedef struct
{
  char* value;
} Name;

typedef struct
{
  char* value;
  u8    opacity;
  COLOR color;
  FONT* font;
} Text;

#define INTERACTABLE_MAX_COMMANDS 5
typedef struct Interatcable
{
  cstr_t commands[INTERACTABLE_MAX_COMMANDS];
  int    num_commands;
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
typedef enum
{
  CHEST_ANIM_CLOSE,
  CHEST_ANIM_OPEN,
} ChestAnimId;

typedef struct
{
  Item items[CHEST_MAX_ITEMS];
  u8   num_items;
} Chest;

typedef struct
{
  u16 mana_points;
  u16 max_mana_points;
} ManaPool;

typedef struct
{
  u16 spell_id;
} AttunementSlot;

typedef struct
{
  int timer;
} Castable;

typedef struct ecs_entity_t Wearer;

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh);

void animation_fini(Animation* anim);

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt);
void      animator_fini(Animator* animator);

void visual_set_anchor_to_center(Visual* v);
void ai_agent_fini(AIAgent* ai_agent);

void hitbox_init(HitBox* h);

void hitbox_init_center(HitBox* h, Vec2 size);
void hitbox_init_bottom_middle(HitBox* h, Vec2 size);

void level_switcher_init(LevelSwitcher* sw, const char* level, const char* dest);
void level_switcher_fini(LevelSwitcher* sw);

void name_init(Name* name, const char* value);
void name_fini(Name* name);

void text_init(Text* text, const char* value, FONT* font, COLOR color);
void text_fini(Text* text);

void visual_init(Visual* v);

#endif // COMPONENTS_H
