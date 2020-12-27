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

typedef enum
{
  CHARACTER_ACTION_NONE,
  CHARACTER_ACTION_REGULAR_ATK,
  CHARACTER_ACTION_SPECIAL_ATK,
} CharacterAction;

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
  NUM_ANIM_STATES,
} AnimationState;

typedef enum
{
  TRANSFORM,
  VISUAL,
  MOTION,
  ANIMATOR,
  CONTROLLER,
  EQUIPMENT,
  WEAPON_ACTION,
  HEATH,
  HITBOX,
  PROJECTILE,
  HEAL_BAR,
  LIFE_SPAN,
  TAG_TO_BE_DESTROYED,
  WEAPON_SKILL_SWING,
  WEAPON_SKILL_CHARGE,
  WEAPON_SKILL_THUNDER_STORM,
  DAMAGE_OUTPUT,
  WEAPON_CORE,
  DROP,
  ITEM_TAG,
  INVULNERABLE,
  ENEMY_TAG,
  PLAYER_TAG,
  CAMERA_TARGET_TAG,
  TILE_COLLISION_TAG,
  CHARACTER_ANIMATOR_TAG,
  AI_AGENT,
  BTV_MOVE_DESTINATION,
  BTV_PATH,
  BTV_FOLLOWING_TARGET,
  SPOT,
  NUM_COMPONENTS
} ComponentId;

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

typedef struct Spot
{
  Vec2  pos;
  float radius;
} Spot;

typedef struct Controller
{
  Direction desired_dir;
  Vec2      desired_vel;
  int       action;
  BOOL      in_action;
  BOOL      lock_dir;
  BOOL      lock_movement;
} Controller;

typedef struct Equipment
{
  ecs_entity_t weapon;
  Vec2         weapon_anchor;
} Equipment;

typedef struct WeponBase
{
  ecs_entity_t wearer;
  s32          atk;
  const char*  name;
} WeaponCore;

/*Weapon skills*/
typedef struct wpskl_Swing
{
  CharacterAction on_action;
  int             timer;
  int             step;
  BOOL            is_active;
} wpskl_Swing;

typedef struct wpskl_Charge
{
  int  on_action;
  int  timer;
  BOOL is_active;
} wpskl_Charge;

typedef struct wpskl_Thust
{
  int on_action;
} wpskl_Thust;

typedef struct wpskl_ThunderStorm
{
  int on_action;
  u16 remaining;
  u16 interval;
  u16 total;
  u16 timer;
} wpskl_ThunderStorm;

typedef struct DamageOutput
{
  int atk;
  int type;
} DamageOutput;

/*entity tag*/
typedef int PlayerTag;

typedef int EnemyTag;

typedef int WeaponTag;

typedef int CameraTargetTag;

typedef int TileCollisionTag;

typedef int ProjectileTag;

typedef int TagToBeDestroyed;

typedef int CharacterAnimatorTag;

typedef struct
{
  ItemId item_id;
} ItemTag;

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

typedef struct HealthBar
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
  ItemId item1;
  ItemId item2;
  int    change1;
  int    change2;
} Drop;

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
  Vec2i nodes[100];
  int   cnt;
  int   curr;
} btv_Path;

typedef ecs_entity_t btv_FollowingTarget;

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

#endif // COMPONENTS_H
