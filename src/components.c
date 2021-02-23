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

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL]                     = ECS_TYPE_EX(Visual, visual_init, NULL, NULL),
  [TRANSFORM]                  = ECS_TYPE_EX(Transform, transform_init, NULL, NULL),
  [ANIMATOR]                   = ECS_TYPE(Animator),
  [PLAYER_TAG]                 = ECS_TYPE(PlayerTag),
  [CONTROLLER]                 = ECS_TYPE(Controller),
  [EQUIPMENT]                  = ECS_TYPE(Equipment),
  [HEALTH]                     = ECS_TYPE(Health),
  [HITBOX]                     = ECS_TYPE_EX(HitBox, hitbox_init, NULL, NULL),
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
  [BRAIN]                      = ECS_TYPE_EX(Brain, NULL, brain_fini, NULL),
  [DESTINATION]                = ECS_TYPE(Destination),
  [PATH]                       = ECS_TYPE(Path),
  [FOLLOWING_TARGET]           = ECS_TYPE(FollowingTarget),
  [SPOT]                       = ECS_TYPE(Spot),
  [WEAPON_SKILL_THUNDER_STORM] = ECS_TYPE(wpskl_ThunderStorm),
  [LADDER_ATTRIBUTES]          = ECS_TYPE(LadderAttributes),
  [NAME]                       = ECS_TYPE(Name),
  [TEXT]                       = ECS_TYPE(Text),
  [INTERACTABLE]               = { .size = sizeof(Interactable) },
  [DIALOGUE]                   = { .size = sizeof(Dialogue) },
  [PICKUPABLE_ATTRIBUTES]      = { .size = sizeof(PickupableAttributes) },
  [MERCHANT]                   = { .size = sizeof(Merchant) },
  [CHEST]                      = ECS_TYPE(Chest),
  [ATTUNEMENT_SLOT]            = ECS_TYPE(AttunementSlot),
  [CASTABLE]                   = ECS_TYPE(Castable),
  [MANA_POOL]                  = ECS_TYPE(ManaPool),
  [WEAPON_SKILL_THUST]         = ECS_TYPE(wpskl_Thust),
  [DOOR_ATTRIBUTES]            = ECS_TYPE(DoorAttributes),
  [REMOVE_IF_OFFSCREEN]        = ECS_TYPE(RemoveIfOffScreen),
  [PROJECTILE_ATTRIBUTES]      = ECS_TYPE(ProjectileAttributes),
  [HOLDER]                     = ECS_TYPE_EX(Holder, holder_init, NULL, NULL),
  [ATTACK_MASK]                = ECS_TYPE(AttackMask),
  [SELF_DESTRUCTION]           = ECS_TYPE(SelfDestruction),
  [ATTACKER]                   = ECS_TYPE(Attacker),
  [WEAPON_SKILL_SHOOT]         = ECS_TYPE(wpskl_Shoot),
};

void visual_set_anchor_to_center(Visual* v)
{
  v->anchor.x = v->sprite.rect.w / 2;
  v->anchor.y = v->sprite.rect.h / 2;
}

void brain_fini(Brain* brain)
{
  bt_node_del(brain->root);
  brain->root = NULL;
}

void hitbox_init(HitBox* h)
{
  h->proxy_id = RTREE_NULL_NODE;
}

void ladder_attrs_init(LadderAttributes* attrs, const char* level, const char* dest)
{
  ASSERT(level != NULL && dest != NULL);
  strncpy(attrs->level, level, LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN);
  attrs->level[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN] = '\0';
  strncpy(attrs->dest, dest, LEVEL_SWITCHER_MAX_DEST_LEN);
  attrs->dest[LEVEL_SWITCHER_MAX_LEVEL_NAME_LEN] = '\0';
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
