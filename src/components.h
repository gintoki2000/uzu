#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "behaviour_tree.h"
#include "path_finder.h"
#include <ecs/ecs.h>
#include <item.h>
#include <toolbox/toolbox.h>
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
  WEAPON_ACTION_NONE,
  WEAPON_ACTION_REGULAR_ATK,
  WEAPON_ACTION_SPECIAL_ATK
};

enum
{
  CHARACTER_ACTION_NONE,
  CHARACTER_ACTION_REGULAR_ATK,
  CHARACTER_ACTION_SPECIAL_ATK,
};

enum
{
  CATEGORY_PLAYER,
  CATEGORY_PROJECTILE,
  CATEGORY_ENEMY,
  CATEGORY_WEAPON,
  CATEGORY_ITEM,
  NUM_CATEGORIES,
};

typedef enum
{
  DAMAGE_TYPE_STRIKE,
  DAMAGE_TYPE_THUST,
  DAMAGE_TYPE_FIRE,
  DAMAGE_TYPE_LIGHTNING,
} DamageType;

typedef struct Motion
{
  Vec2  vel;
  Vec2  acc;
  float max_speed;
  float max_force;
} Motion;
typedef struct Transform
{
  Vec2   prev_pos;
  Vec2   pos;
  double rot;
  BOOL   was_changed;
} Transform;

typedef struct Visual
{
  SDL_Point        anchor;
  SDL_RendererFlip flip;
  Sprite           sprite;
} Visual;

typedef struct Animation
{
  s32         frame_duration;
  SpriteSheet sheet;
} Animation;

typedef struct CharacterAction
{
  int desired_dir;
  int action;
} CharacterAction;


typedef struct Equipment
{
  ecs_entity_t rhand;
} Equipment;

typedef struct WeponBase
{
  s32         atk;
  const char* name;
  BOOL        in_action;
} WeaponCore;

typedef struct Swingable
{
  int  on_action;
  int  timer;
  int  step;
  BOOL is_active;
} Swingable;

typedef struct Thustable
{
  int on_action;
} Thustable;

typedef struct ThunderStrike
{
  int on_action;
} ThundeStrike;

typedef struct DamageOutput
{
  int atk;
  int type;
} DamageOutput;

typedef struct WeaponAction
{
  int action;
} WeaponAction;

typedef int PlayerTag;

typedef int EnemyTag;

typedef int WeaponTag;

typedef int CameraTargetTag;

typedef int TileCollisionTag;

typedef struct
{
  ItemId item_id;
} ItemTag;

typedef int ProjectileTag;

typedef struct Projectile
{
  int atk;
} Projectile;

typedef struct Heath
{
  s32 hit_points;
  s32 max_hit_points;
} Heath;

typedef struct Animator
{
  u32        current_anim;
  u32        elapsed;
  u32        cnt;
  Animation* anims;
} Animator;

typedef struct HitBox
{
  Vec2 size;
  Vec2 anchor;
  int  proxy_id;
  u16  mask_bits;
  u16  category;
  BOOL check_tile_collision;
} HitBox;

typedef struct HealBar
{
  int       len; // in pixel
  SDL_Color color;
  SDL_Color border;
  SDL_Point anchor;
} HealBar;

typedef struct LifeSpan
{
  s32 remaining;
} LifeSpan;

typedef struct
{
  ItemId item1;
  ItemId item2;
  int    change1;
  int    change2;
} Drop;

typedef int TagToBeDestroyed;

typedef struct
{
  int remaining;
} Invulnerable;

typedef struct
{
  bt_Node* root;
} AIAgent;

//** BT Vars

typedef Vec2 btv_MoveDestination;
typedef struct
{
  pf_Node nodes[100];
  int     cnt;
  int     curr;
} btv_Path;

enum
{
  TRANSFORM,
  VISUAL,
  MOTION,
  PLAYER_TAG,
  ANIMATOR,
  CHARACTER_ACTION,
  EQUIPMENT,
  WEAPON_ACTION,
  HEATH,
  HITBOX,
  ENEMY_TAG,
  PROJECTILE,
  HEAL_BAR,
  LIFE_SPAN,
  TAG_TO_BE_DESTROYED,
  SWINGABLE,
  THUNDER_STRIKE,
  DAMAGE_OUTPUT,
  WEAPON_CORE,
  DROP,
  ITEM_TAG,
  INVULNERABLE,
  CAMERA_TARGET_TAG,
  AI_AGENT,
  BTV_MOVE_DESTINATION,
  BTV_PATH,
  NUM_COMPONENTS
};

typedef enum
{
  ANIM_STATE_IDLE,
  ANIM_STATE_RUN,
  ANIM_STATE_HIT,
  NUM_ANIM_STATES,
} AnimationState;

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh);

void animation_fini(Animation* anim);

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt);
void      animator_fini(Animator* animator);

void visual_set_anchor_to_center(Visual* v);
void ai_agent_fini(AIAgent* ai_agent);

#endif // COMPONENTS_H
