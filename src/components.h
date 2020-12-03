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
  SWORD_STATE_IDLE,
  SWORD_STATE_REGULAR_ATK
};

enum
{
  AXE_STATE_IDLE,
  AXE_STATE_REGULAR_ATK,
  AXE_STATE_HEAVY_ATK,
};

typedef struct Motion
{
  Vec2 vel;
  Vec2 acc;
} Motion;
typedef struct Transform
{
  Vec2   pos;
  double rot;
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

typedef struct GenericSword
{
  s32 atk;
  s32 range;
  s32 state;
  u32 timer;
  u32 step;
} GenericSword;

typedef struct GenericAxe
{
  s32 atk;
  s32 range;
  s32 state;
  u32 timer;
  u32 step;
} GenericAxe;

typedef struct GenericSpear
{
  s32 atk;
} GenericSpear;

typedef struct GoldenSword
{
  s32 state;
  u32 timer;
  u32 step;
} GoldenSword;

typedef struct GenericBow
{
  s32 atk;
} GenericBow;

typedef struct MagicStaff
{
  s32 mpcost;
  ecs_entity_t (*projectile_factory_fn)(Ecs*);
} MagicStaff;

typedef struct WeaponAction
{
  int action;
} WeaponAction;

typedef int PlayerTag;

typedef int EnemyTag;

typedef struct Projectile
{
  int atk;
} Projectile;

typedef struct PlayerWeaponTag
{
  int dummy;
} PlayerWeaponTag;

typedef struct Heath
{
  u32 hit_points;
  u32 max_hit_points;
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
  int proxy_id;
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
  GENERIC_SWORD,
  HEATH,
  GENERIC_AXE,
  GENERIC_BOW,
  HITBOX,
  ENEMY_TAG,
  PROJECTILE,
  PLAYER_WEAPON_TAG,
  HEAL_BAR,
  LIFE_SPAN,
  TAG_TO_BE_DESTROYED,
  GOLDEN_SWORD,
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
