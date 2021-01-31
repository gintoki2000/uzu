#include "components.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox/sprite-sheet.h>

#define ECS_TYPE(__T)                                                                              \
  {                                                                                                \
    .size = sizeof(__T)                                                                            \
  }
#define ECS_TYPE_EX(__T, __init_fn, __fini_fn)                                                     \
  {                                                                                                \
    .size = sizeof(__T), .init_fn = (ecs_comp_init_fn_t)__init_fn,                                 \
    .fini_fn = (ecs_comp_fini_fn_t)__fini_fn                                                       \
  }

static void transform_init(Transform* transform);
static void holder_init(Holder* holder);

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL]                     = ECS_TYPE_EX(Visual, visual_init, NULL),
  [TRANSFORM]                  = ECS_TYPE_EX(Transform, transform_init, NULL),
  [ANIMATOR]                   = ECS_TYPE_EX(Animator, NULL, animator_fini),
  [PLAYER_TAG]                 = ECS_TYPE(PlayerTag),
  [CONTROLLER]                 = ECS_TYPE(Controller),
  [EQUIPMENT]                  = ECS_TYPE(Equipment),
  [HEALTH]                     = ECS_TYPE(Health),
  [HITBOX]                     = ECS_TYPE_EX(HitBox, hitbox_init, NULL),
  [ENEMY_TAG]                  = ECS_TYPE(PlayerTag),
  [ENDABLE_TILE_COLLISION_TAG] = ECS_TYPE(EnableTileCollisionTag),
  [CHARACTER_ANIMATOR_TAG]     = ECS_TYPE(CharacterAnimatorTag),
  [HEAL_BAR]                   = ECS_TYPE(HealthBar),
  [LIFE_SPAN]                  = ECS_TYPE(LifeSpan),
  [MOTION]                     = ECS_TYPE(Motion),
  [DESTROYED_TAG]              = ECS_TYPE(DestroyedTag),
  [WEAPON_ATTRIBUTES]          = ECS_TYPE(WeaponAttributes),
  [DAMAGE_OUTPUT]              = ECS_TYPE(DamageOutput),
  [WEAPON_SKILL_SWING]         = ECS_TYPE(wpskl_Swing),
  [WEAPON_SKILL_CHARGE]        = ECS_TYPE(wpskl_Charge),
  [DROP]                       = ECS_TYPE(Drop),
  [INVULNERABLE]               = ECS_TYPE(Invulnerable),
  [INPUT_BLOCKER]              = ECS_TYPE(InputBlocker),
  [CAMERA_TARGET_TAG]          = ECS_TYPE(CameraTargetTag),
  [BRAIN]                      = ECS_TYPE_EX(Brain, NULL, brain_fini),
  [DESTINATION]                = ECS_TYPE(Destination),
  [PATH]                       = ECS_TYPE(Path),
  [FOLLOWING_TARGET]           = ECS_TYPE(FollowingTarget),
  [SPOT]                       = ECS_TYPE(Spot),
  [WEAPON_SKILL_THUNDER_STORM] = ECS_TYPE(wpskl_ThunderStorm),
  [LEVEL_SWITCHER]             = ECS_TYPE(LevelSwitcher),
  [NAME]                       = ECS_TYPE(Name),
  [TEXT]                       = ECS_TYPE(Text),
  [INTERACTABLE]               = { .size = sizeof(Interactable) },
  [DIALOGUE]                   = { .size = sizeof(Dialogue) },
  [ITEM_TAG]                   = { .size = sizeof(ItemTag) },
  [MERCHANT]                   = { .size = sizeof(Merchant) },
  [CHEST]                      = ECS_TYPE(Chest),
  [ATTUNEMENT_SLOT]            = ECS_TYPE(AttunementSlot),
  [CASTABLE]                   = ECS_TYPE(Castable),
  [MANA_POOL]                  = ECS_TYPE(ManaPool),
  [WEAPON_SKILL_THUST]         = ECS_TYPE(wpskl_Thust),
  [DOOR_INFO]                  = ECS_TYPE(DoorInfo),
  [REMOVE_IF_OFFSCREEN]        = ECS_TYPE(RemoveIfOffScreen),
  [PROJECTILE_ATTRIBUTES]      = ECS_TYPE(ProjectileAttributes),
  [HOLDER]                     = ECS_TYPE_EX(Holder, holder_init, NULL),
};

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh)
{
  sprite_sheet_init_1(&anim->sheet, tex, x, y, sw, sh, row, col);
  anim->frame_duration = 1;
  return anim;
}

void animation_fini(Animation* anim)
{
  sprite_sheet_fini(&anim->sheet);
}

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt)
{
  animator->anims        = malloc(cnt * sizeof(Animation));
  animator->cnt          = cnt;
  animator->elapsed      = 0;
  animator->current_anim = 0;
  memmove(animator->anims, anims, cnt * sizeof(Animation));
  return animator;
}

static void transform_init(Transform* transform)
{
  *transform = (Transform){ .hdir = 1 };
}

void animator_fini(Animator* animator)
{
  for (u32 i = 0; i < animator->cnt; ++i)
  {
    animation_fini(&animator->anims[i]);
  }
  free(animator->anims);
}

void visual_set_anchor_to_center(Visual* v)
{
  v->anchor.x = v->sprite.rect.w / 2;
  v->anchor.y = v->sprite.rect.h / 2;
}

void brain_fini(Brain* ai_agent)
{
  bt_node_del(ai_agent->root);
  ai_agent->root = NULL;
}

void hitbox_init(HitBox* h)
{
  h->proxy_id = RTREE_NULL_NODE;
}

void level_switcher_init(LevelSwitcher* sw, const char* level, const char* dest)
{
  ASSERT(level != NULL && dest != NULL);
  strncpy(sw->level, level, LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN);
  sw->level[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN] = '\0';
  strncpy(sw->dest, dest, LEVEL_SWITCHER_MAX_DEST_LEN);
  sw->dest[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN] = '\0';
}

void name_init(Name* name, const char* value)
{
  ASSERT(value != NULL);
  strncpy(name->value, value, NAME_MAX_LEN);
  name->value[NAME_MAX_LEN] = '\0';
}

void text_init(Text* text, const char* value, FONT* font, COLOR color)
{
  ASSERT(value != NULL);
  strncpy(text->value, value, TEXT_MAX_LEN);
  text->value[TEXT_MAX_LEN] = '\0';
  text->font                = font;
  text->effect.color        = color;
  text->effect.alignment    = FC_ALIGN_CENTER;
  text->effect.scale.x      = 1.f;
  text->effect.scale.y      = 1.f;
}

void visual_init(Visual* v)
{
  v->opacity = 255;
  v->color   = (COLOR){ 255, 255, 255, 255 };
  v->anchor  = (POINT){ 0, 0 };
  v->sprite  = (Sprite){ .tex = NULL };
  v->flip    = SDL_FLIP_NONE;
}

static void holder_init(Holder* holder)
{
  *holder = (Holder){ .value = ECS_NULL_ENT };
}
