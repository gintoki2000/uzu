#include "components.h"
#include <stdlib.h>
#include <string.h>

#define ECS_TYPE(__T)                                                                              \
  {                                                                                                \
    .size = sizeof(__T)                                                                            \
  }
#define ECS_TYPE_EX(__T, __init_fn, __fini_fn, __cpy_fn)                                           \
  {                                                                                                \
    .size = sizeof(__T), .init_fn = (ecs_comp_init_fn_t)__init_fn,                                 \
    .fini_fn = (ecs_comp_fini_fn_t)__fini_fn, .cpy_fn = (ecs_comp_cpy_fn_t)__cpy_fn,               \
  }

static void transform_init(Transform* transform);
static void holder_init(Holder* holder);
static void visual_init(Visual* v);
static void brain_fini(Brain* ai_agent);
static void hitbox_init(HitBox* h);

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL]                       = ECS_TYPE_EX(Visual, visual_init, NULL, NULL),
  [TRANSFORM]                    = ECS_TYPE_EX(Transform, transform_init, NULL, NULL),
  [ANIMATOR]                     = ECS_TYPE(Animator),
  [PLAYER_TAG]                   = ECS_TYPE(PlayerTag),
  [CONTROLLER]                   = ECS_TYPE(Controller),
  [EQUIPMENT]                    = ECS_TYPE(Equipment),
  [HEALTH]                       = ECS_TYPE(Health),
  [HITBOX]                       = ECS_TYPE_EX(HitBox, hitbox_init, NULL, NULL),
  [ENEMY_TAG]                    = ECS_TYPE(PlayerTag),
  [ENABLE_TILE_COLLISION_TAG]    = ECS_TYPE(EnableTileCollisionTag),
  [CHARACTER_ANIMATOR_TAG]       = ECS_TYPE(CharacterAnimatorTag),
  [HEAL_BAR]                     = ECS_TYPE(HealthBar),
  [LIFE_SPAN]                    = ECS_TYPE(LifeSpan),
  [MOTION]                       = ECS_TYPE(Motion),
  [DESTROYED_TAG]                = ECS_TYPE(DestroyedTag),
  [WEAPON_ATTRIBUTES]            = ECS_TYPE(WeaponAttributes),
  [WEAPON_SWING_ATTACK]          = ECS_TYPE(WeaponSwingAttack),
  [WEAPON_CHARGE_ATTACK]         = ECS_TYPE(WeaponChargeAttack),
  [DROP]                         = ECS_TYPE(Drop),
  [INVULNERABLE]                 = ECS_TYPE(Invulnerable),
  [INPUT_BLOCKER]                = ECS_TYPE(InputBlocker),
  [CAMERA_TARGET_TAG]            = ECS_TYPE(CameraTargetTag),
  [BRAIN]                        = ECS_TYPE_EX(Brain, NULL, brain_fini, NULL),
  [DESTINATION]                  = ECS_TYPE(Destination),
  [PATH]                         = ECS_TYPE(Path),
  [FOLLOWING_TARGET]             = ECS_TYPE(FollowingTarget),
  [SPOT]                         = ECS_TYPE(Spot),
  [WEAPON_THUNDER_STORM_RELEASE] = ECS_TYPE(WeaponThunderStormRelease),
  [LADDER_ATTRIBUTES]            = ECS_TYPE(LadderAttributes),
  [NAME]                         = ECS_TYPE(Name),
  [TEXT]                         = ECS_TYPE(Text),
  [INTERACTABLE]                 = { .size = sizeof(Interactable) },
  [DIALOGUE]                     = { .size = sizeof(Dialogue) },
  [PICKUPABLE_ATTRIBUTES]        = { .size = sizeof(PickupableAttributes) },
  [MERCHANT]                     = { .size = sizeof(Merchant) },
  [CHEST]                        = ECS_TYPE(Chest),
  [ATTUNEMENT_SLOT]              = ECS_TYPE(AttunementSlot),
  [WEAPON_CAST]                  = ECS_TYPE(Castable),
  [MANA_POOL]                    = ECS_TYPE(ManaPool),
  [WEAPON_THUST_ATTACK]          = ECS_TYPE(WeaponThustAttack),
  [DOOR_ATTRIBUTES]              = ECS_TYPE(DoorAttributes),
  [REMOVE_IF_OFFSCREEN]          = ECS_TYPE(RemoveIfOffScreen),
  [PROJECTILE_ATTRIBUTES]        = ECS_TYPE(ProjectileAttributes),
  [HOLDER]                       = ECS_TYPE_EX(Holder, holder_init, NULL, NULL),
  [ATTACK_MASK]                  = ECS_TYPE(AttackMask),
  [SELF_DESTRUCTION]             = ECS_TYPE(SelfDestruction),
  [ATTACKER]                     = ECS_TYPE(Attacker),
  [WEAPON_SHOOT]                 = ECS_TYPE(WeaponShoot),
  [MOVE_SPEED]                   = ECS_TYPE(MoveSpeed),
};

void brain_fini(Brain* brain)
{
  bt_node_del(brain->root);
  brain->root = NULL;
}

void hitbox_init(HitBox* h)
{
  h->proxy_id = RTREE_NULL_NODE;
  h->anchor = (Vec2){ 0 };
  h->category = 0;
  h->size = (Vec2){ 16.f, 16.f };
  h->mask_bits = 0xffff;
}

void ladder_attrs_init(LadderAttributes* attrs, const char* level, const char* dest)
{
  ASSERT(level != NULL && dest != NULL);
  SDL_strlcpy(attrs->level, level, LADDER_ATTRS_MAX_LEVEL_NAME_LEN);
  SDL_strlcpy(attrs->dest, dest, LADDER_ATTRS_MAX_LEVEL_NAME_LEN);
}

void name_init(Name* name, const char* value)
{
  ASSERT(value != NULL);
  SDL_strlcpy(name->value, value, NAME_MAX_LEN);
}

void text_init(Text* text, const char* value, FONT* font, COLOR color)
{
  ASSERT(value != NULL && font != NULL);
  SDL_strlcpy(text->value, value, TEXT_MAX_LEN);
  text->font             = font;
  text->effect.color     = color;
  text->effect.alignment = FC_ALIGN_CENTER;
  text->effect.scale.x   = 1.f;
  text->effect.scale.y   = 1.f;
}

void visual_init(Visual* v)
{
  v->opacity = 255;
  v->color   = (COLOR){ 255, 255, 255, 255 };
  v->anchor  = (POINT){ 0, 0 };
  v->sprite  = (Sprite){ 0 };
  v->flip    = SDL_FLIP_NONE;
}

static void holder_init(Holder* holder)
{
  holder->value = ECS_NULL_ENT;
}

static void transform_init(Transform* t)
{
  t->hdir          = 1;
  t->prev_position = t->position = (Vec2){ 0.f, 0.f };
  t->rotation                    = 0.0;
  t->z                           = 0.f;
  t->lock_hdir                   = 0;
}
