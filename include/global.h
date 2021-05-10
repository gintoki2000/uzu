#ifndef GLOBAL_H
#define GLOBAL_H

#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

typedef enum
{
  NONE,
  UP,
  DOWN,
  LEFT,
  RIGHT
} Direction;

enum
{
  CATEGORY_PLAYER,      // 0
  CATEGORY_PROJECTILE,  // 1
  CATEGORY_ENEMY,       // 2
  CATEGORY_WEAPON,      // 3
  CATEGORY_ITEM,        // 4
  CATEGORY_LADDER,      // 5
  CATEGORY_INTERACABLE, // 6
  CATEGORY_TRIGGER,
  NUM_CATEGORIES,
};

typedef enum
{
  DAMAGE_TYPE_STRIKE,
  DAMAGE_TYPE_THUST,
  DAMAGE_TYPE_FIRE,
  DAMAGE_TYPE_LIGHTNING,
  DAMAGE_TYPE_ICE,
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

typedef struct Sprite
{
  u16      texture_id;
  SDL_Rect rect;
} Sprite, Icon;

void sprite_init(Sprite* s, u16 texture_id);

typedef struct SpriteSheet
{
  u16 texture_id;
  u16 offset_x;
  u16 offset_y;
  u16 sprite_width;
  u16 sprite_height;
  u16 count;
} SpriteSheet;

Sprite sprite_sheet_at(const SpriteSheet* sheet, int index);

typedef struct Animation
{
  u16 frame_duration;
  u16 mode;
  u16 texture_id;
  u16 offset_x;
  u16 offset_y;
  u16 sprite_width;
  u16 sprite_height;
  u16 count;
} Animation;

Sprite animation_keyframe(const Animation* animation, u16 elapsed);

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
  ITEM_TYPE_SCROLL_ICE_ARROW,
  ITEM_TYPE_SCROLL_FIRE_BALL,
  ITEM_TYPE_KEY_1_1,
  ITEM_TYPE_ANIME_SWORD,
  ITEM_TYPE_SPEAR,
  ITEM_TYPE_STAFF,
  ITEM_TYPE_CLEAVER,
  NUM_ITEM_TYPES,
} ItemTypeId;
#define ITEM_TYPE_ID_NULL NUM_ITEM_TYPES

// PickupableEntity ID
typedef enum
{
  PICKUPABLE_RED_FLASK,
  PICKUPABLE_BIG_RED_FLASK,
  PICKUPABLE_BLUE_FLASK,
  PICKUPABLE_COIN,
  PICKUPABLE_KEY_1_1,
  NUM_PICKUPABLE_TYPES,
} PickupableType;
#define PICKUPABLE_ID_NULL NUM_PICKUPABLE_TYPES

typedef enum
{
  ITEM_CATEGORY_CONSUMABLE,
  ITEM_CATEGORY_EQUIPMENT,
  ITEM_CATEGORY_SCROLL,
  ITEM_CATEGORY_KEY,
  NUM_ITEM_CATEGORIES,
} ItemCategory;

typedef struct ItemType
{
  const char*  name;
  const char*  description;
  ItemCategory category;
  BOOL         stackable;
  void (*use)(const void* data, Ecs*, ecs_entity_t);
  const void* data;
  Icon        icon;
} ItemType;

typedef struct Item
{
  ItemTypeId type;
  u8         quality;
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

typedef enum
{
  SPELL_ICE_ARROW,
  SPELL_FIRE_BALL,
  SPELL_HOMIE_FIRE,
  NUM_SPELLS
} SpellType;
#define SPELL_ID_NULL NUM_SPELLS

typedef struct Spell
{
  const char* name;
  void (*cast)(Ecs*, ecs_entity_t, ecs_entity_t);
  BOOL (*process)(Ecs*, ecs_entity_t, ecs_entity_t);
  u16  cost;
  u16  cast_spd;
  Icon icon;
} Spell;

typedef enum
{
  WEAPON_SPEAR,
  WEAPON_CLEAVER,
  WEAPON_LAVIS_SWORD,
  WEAPON_ANIME_SWORD,
  WEAPON_STAFF,
  WEAPON_BOW,
  WEAPON_KATANA,
  NUM_WEAPONS,
} WeaponType;
#define WEAPON_ID_NULL NUM_WEAPONS

typedef enum
{
  JOB_KNIGHT,
  JOB_DRAGON,
  JOB_WIZZARD,
  JOB_ELF,
  NUM_JOBS,
} JobType;

typedef enum
{
  PROJECTILE_ICE_ARROW,
  PROJECTILE_FIRE_BALL,
  NUM_PROJECTILE_TYPES
} ProjecttileType;

typedef enum
{
  CAST_EFFECT_ICE,
  CAST_EFFECT_FIRE,
  CAST_EFFECT_LIGHTNING,
  NUM_CAST_EFFECTS
} CastEffectType;

typedef struct Cursor
{
  Sprite    sprite;
  SDL_Point hotspot;
} Cursor;

enum StatusEffectType
{
  STATUS_EFFECT_FREEZED,
  STATUS_EFFECT_POISONED,
  STATUS_EFFECT_PARALYZED,
  STATUS_EFFECT_BURNED,
  NUM_STATUS_EFFECT,
};

extern ecs_entity_t (*const gMakeWeaponFnTbl[NUM_WEAPONS])(Ecs*);
extern ecs_entity_t (*const gMakeCharacterFnTbl[NUM_JOBS])(Ecs*, Vec2);
extern ecs_entity_t (*const gMakePickupableFnTbl[NUM_ITEM_TYPES])(Ecs*, Vec2 pos);
extern ecs_entity_t (*const gMakeProjectileFn[])(Ecs*, Vec2 pos, Vec2 dir, u16 mask);
extern ecs_entity_t (*const gMakeCastEffectFnTbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos);
extern const ItemType g_item_types[NUM_ITEM_TYPES];
extern const Spell    gSpellTbl[NUM_SPELLS];
extern const u16      gPickupableToItemTypeIdTbl[];

Conversation* conversation_init(Conversation* self);
void          conversation_fini(Conversation* self);
#endif // GLOBAL_H
