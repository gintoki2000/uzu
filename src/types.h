#ifndef TYPES_H
#define TYPES_H

#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

typedef enum
{
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

typedef enum
{
  CHARACTER_ACTION_NONE,
  CHARACTER_ACTION_REGULAR_ATK,
  CHARACTER_ACTION_SPECIAL_ATK,
} CharacterAction;

enum
{
  CATEGORY_PLAYER,      // 0
  CATEGORY_PROJECTILE,  // 1
  CATEGORY_ENEMY,       // 2
  CATEGORY_WEAPON,      // 3
  CATEGORY_ITEM,        // 4
  CATEGORY_LADDER,      // 5
  CATEGORY_INTERACABLE, // 6
  NUM_CATEGORIES,
};

typedef enum
{
  DAMAGE_TYPE_STRIKE,
  DAMAGE_TYPE_THUST,
  DAMAGE_TYPE_FIRE,
  DAMAGE_TYPE_LIGHTNING,
} DamageType;

typedef enum
{
  ENEMY_TYPE_HUGE_DEMON,
  ENEMY_TYPE_CHORT,
} EnemyType;

typedef enum
{
  ANIM_STATE_IDLE,
  ANIM_STATE_RUN,
  ANIM_STATE_HIT,
  ANIM_STATE_JUMP,
  NUM_ANIM_STATES,
} AnimationState;

typedef struct
{
  char*     name;
  PtrArray* sentences;
  PtrArray* responses;
} Conversation;

#define ITEM_MAX_STACK 255

typedef enum
{
  ITEM_TYPE_RED_FLASK,
  ITEM_TYPE_BIG_RED_FLASK,
  ITEM_TYPE_BLUE_FLASK,
  NUM_ITEM_TYPES,
} ItemTypeId;

typedef enum
{
  ITEM_CATEGORY_CONSUMABLE,
  ITEM_CATEGORY_EQUIPMENT,
  NUM_ITEM_CATEGORIES,
} ItemCategory;

typedef struct
{
  const char*  name;
  const char*  description;
  ItemCategory category;
  BOOL         stackable;
  void (*on_use)(Ecs*, ecs_entity_t);
  struct
  {
    u32  texture_id;
    RECT rect;
  } sprite;

} ItemType;

typedef struct Item
{
  ItemTypeId type_id;
  u8         num_items;
} Item;

#define ITEM_PAYLOAD_INFINTE -1
typedef struct
{
  ItemTypeId type_id;
  s16        available;
  u16        price;
} ItemPayload;

typedef enum
{
  ANIM_PLAY_MODE_NORMAL,
  ANIM_PLAY_MODE_LOOP,
} AnimationPlayMode;

typedef struct
{
  u16         frame_duration;
  u16         mode;
  SpriteSheet sheet;
} Animation;

typedef struct
{
  const char* name;
  void (*cast_fn)(Ecs* ecs, ecs_entity_t caster);
  u16 cost;
  u16 cast_spd;
} Spell;

enum
{
  WEAPON_SPEAR,
  WEAPON_CLEAVER,
  WEAPON_LAVIS_SWORD,
  WEAPON_ANIME_SWORD,
  WEAPON_STAFF,
  WEAPON_BOW,
  NUM_WEAPONS,
} WeaponType;

enum
{
  JOB_KNIGHT,
  JOB_DRAGON,
  JOB_WIZZARD,
  JOB_HUNTER,
  NUM_CLASSES,
} JobType;

extern ecs_entity_t (*const g_weapon_create_fn_tbl[NUM_WEAPONS])(Ecs*, u16);
extern ecs_entity_t (*const g_char_create_fn_tbl[NUM_CLASSES])(Ecs*, Vec2);
extern ecs_entity_t (*const g_item_create_fn_tbl[NUM_ITEM_TYPES])(Ecs*, Vec2 pos);
extern const ItemType g_item_types[];

Conversation* conversation_init(Conversation* self);
void          conversation_fini(Conversation* self);
#endif // TYPES_H
