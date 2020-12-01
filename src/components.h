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

typedef enum
{
  WEAPON_ACTION_NONE,
  WEAPON_ACTION_REGULAR_ATK,
  WEAPON_ACTION_SPECIAL_ATK
} WeaponAction;

typedef enum
{
  SWORD_STATE_IDLE,
  SWORD_STATE_REGULAR_ATK
} SwordState;

typedef enum
{
  AXE_STATE_IDLE,
  AXE_STATE_REGULAR_ATK,
  AXE_STATE_HEAVY_ATK,
} SwordAxe;

typedef struct Transform
{
  s32    x;
  s32    y;
  s32    local_pos_x;
  s32    local_pos_y;
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

typedef struct CmdInput
{
  int desired_dir;
  int action;
} CmdInput;

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

typedef struct LevinSword
{
  s32 state;
  u32 timer;
  u32 step;
} LevinSword;

typedef struct MagicStaff
{
  s32 mpcost;
  ecs_entity_t (*projectile_factory_fn)(Ecs*);
} MagicStaff;

typedef struct WeaponCmdInput
{
  int action;
} WeaponCmdInput;

typedef struct PlayerTag
{
  int dummy;
} PlayerTag;

typedef struct EnemyTag
{
  int dummy;
} EnemyTag;

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
} HitBox;

typedef struct HealBar
{
  int       len; // in pixel
  SDL_Color color;
  SDL_Color border;
} HealBar;

typedef struct LifeSpan
{
  s32 remaining;
} LifeSpan;

enum
{
  TRANSFORM,
  VISUAL,
  PLAYER_TAG,
  ANIMATOR,
  COMMAND_INPUT,
  EQUIPMENT,
  WP_CMD_INPUT,
  GENERIC_SWORD,
  HEATH,
  GENERIC_AXE,
  HITBOX,
  ENEMY_TAG,
  PLAYER_WEAPON_TAG,
  HEAL_BAR,
  LIFE_SPAN,
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

#endif // COMPONENTS_H
