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

typedef enum
{
  SPELL_ICE_ARROW,
  SPELL_FIRE_BALL,
  NUM_SPELLS
} SpellType;

typedef struct Spell
{
  const char* name;
  void (*cast)(Ecs* ecs, ecs_entity_t caster, ecs_entity_t weapon);
  u16 cost;
  u16 cast_spd;
  u16 casting_effect;
} Spell;

typedef enum
{
  WEAPON_SPEAR,
  WEAPON_CLEAVER,
  WEAPON_LAVIS_SWORD,
  WEAPON_ANIME_SWORD,
  WEAPON_STAFF,
  WEAPON_BOW,
  NUM_WEAPONS,
} WeaponType;

typedef enum
{
  JOB_KNIGHT,
  JOB_DRAGON,
  JOB_WIZZARD,
  JOB_HUNTER,
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

extern ecs_entity_t (*const g_weapon_create_fn_tbl[NUM_WEAPONS])(Ecs*, u16);
extern ecs_entity_t (*const g_char_create_fn_tbl[NUM_JOBS])(Ecs*, Vec2);
extern ecs_entity_t (*const g_item_create_fn_tbl[NUM_ITEM_TYPES])(Ecs*, Vec2 pos);
extern ecs_entity_t (*const g_projectile_create_fn[])(Ecs*, Vec2 pos, Vec2 dir, u16 mask);
extern ecs_entity_t (*const g_cast_effect_fn_tbl[NUM_CAST_EFFECTS])(Ecs* ecs, Vec2 pos);
extern const ItemType g_item_types[NUM_ITEM_TYPES];
extern const Spell    g_spell_tbl[NUM_SPELLS];

Conversation* conversation_init(Conversation* self);
void          conversation_fini(Conversation* self);
#endif // TYPES_H
