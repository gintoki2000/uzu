#ifndef COMPONENTS_H
#define COMPONENTS_H
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

enum
{
  WEAPON_ACTION_NONE,
  WEAPON_ACTION_REGULAR_ATK,
  WEAPON_ACTION_SPECIAL_ATK
};

enum
{
  CATEGORY_PLAYER,
  CATEGORY_PLAYER_PROJECTILE,
  CATEGORY_ENEMY,
  CATEGORY_ENEMY_PROJECTILE,
  CATEGORY_PLAYER_WEAPON,
  CATEGORY_ENEMY_WEAPON,
  NUM_CATEGORIES,
};

typedef struct Motion
{
  Vec2 vel;
  Vec2 acc;
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

typedef struct CharacterStat
{
  float move_speed;
  int   str;
  int   mag;
} CharacterStats;

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
} DamageOutput;

typedef struct WeaponAction
{
  int action;
} WeaponAction;

typedef int PlayerTag;

typedef int EnemyTag;

typedef int WeaponTag;

typedef int ItemTag;

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

typedef int TagToBeDestroyed;

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
  CHARACTER_STATS,
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

INLINE void visual_set_anchor_to_center(Visual* v)
{
  v->anchor.x = v->sprite.rect.w / 2;
  v->anchor.y = v->sprite.rect.h / 2;
}

#endif // COMPONENTS_H
