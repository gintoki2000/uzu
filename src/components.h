#ifndef COMPONENTS_H
#define COMPONENTS_H
#include "SDL_FontCache.h"
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
  DESTINATION,
  PATH,
  FOLLOWING_TARGET,
  SPOT,
  LEVEL_SWITCHER,
  NAME,
  INTERACTABLE,
  TEXT,
  NUM_COMPONENTS
} ComponentId;

typedef struct
{
  Vec2  vel;
  Vec2  acc;
  float max_speed;
  float max_force;
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
  s32         frame_duration;
  SpriteSheet sheet;
} Animation;

typedef struct
{
  Vec2  pos;
  float radius;
} Spot;

typedef struct
{
  Direction desired_dir;
  Vec2      desired_vel;
  int       action;
  BOOL      in_action;
  BOOL      lock_dir;
  BOOL      lock_movement;
} Controller;

typedef struct
{
  ecs_entity_t weapon;
  Vec2         weapon_anchor;
} Equipment;

typedef struct
{
  ecs_entity_t wearer;
  s32          atk;
  const char*  name;
} WeaponCore;

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
  int  on_action;
  int  timer;
  BOOL is_active;
} wpskl_Charge;

typedef struct
{
  int on_action;
} wpskl_Thust;

typedef struct
{
  int on_action;
  u16 remaining;
  u16 interval;
  u16 total;
  u16 timer;
} wpskl_ThunderStorm;

typedef struct
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

typedef int NpcTag;

typedef int TagToBeDestroyed;

typedef int CharacterAnimatorTag;

typedef struct
{
  ItemTypeId item_id;
} ItemTag;

typedef struct
{
  int atk;
} Projectile;

typedef struct
{
  s32 hit_points;
  s32 max_hit_points;
} Heath;

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
} Invulnerable;

typedef struct
{
  BTNode* root;
} AIAgent;

//** BT Vars

typedef Vec2 Destination;
typedef struct
{
  Vec2i nodes[100];
  int   cnt;
  int   curr;
} Path;

typedef ecs_entity_t FollowingTarget;

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

typedef struct
{
  char text[5][10];
  int  cnt;
} Interactable;

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

void text_init(Text* text, const char* value, const FONT* font, COLOR color);
void text_fini(Text* text);

#endif // COMPONENTS_H
