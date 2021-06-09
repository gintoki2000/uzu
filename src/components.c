#include "components.h"
#include <stdlib.h>
#include <string.h>

#define COMPONENT_DESC(__T)                                                                        \
  {                                                                                                \
    .size = sizeof(__T)                                                                            \
  }
#define COMPONENT_DESC_EX(__T, __init_fn, __fini_fn, __cpy_fn)                                     \
  {                                                                                                \
    .size = sizeof(__T), .initFunc = (ecs_CompInitFunc)__init_fn,                                 \
    .finiFunc = (ecs_CompFiniFunc)__fini_fn, .cpyFunc = (ecs_CompCpyFunc)__cpy_fn,               \
  }

static void transform_init(Transform* transform);
static void holder_init(Holder* holder);
static void visual_init(Visual* v);
static void brain_fini(Brain* ai_agent);
static void hitbox_init(HitBox* h);
static void aim_direction_init(AimDirection* fd);
static void attack_target_init(AttackTarget* attack_target);
static void script_fini(Script* script);

void delete_action(void* action);

const ecs_CompDesc gCompDescs[NUM_COMPONENTS] = {
  [AGGRO_AREA]                   = COMPONENT_DESC(AggroArea),
  [AGILITY_CHANGED]              = COMPONENT_DESC(AgilityChanged),
  [AIM_DIRECTION]                = COMPONENT_DESC_EX(AimDirection, aim_direction_init, NULL, NULL),
  [ANIMATOR]                     = COMPONENT_DESC(Animator),
  [ATTACKER]                     = COMPONENT_DESC(Attacker),
  [ATTACK_COMMAND]               = COMPONENT_DESC(AttackCommand),
  [ATTACK_MASK]                  = COMPONENT_DESC(AttackMask),
  [ATTACK_TARGET]                = COMPONENT_DESC_EX(AttackTarget, attack_target_init, NULL, NULL),
  [ATTUNEMENT_SLOT]              = COMPONENT_DESC(AttunementSlot),
  [BRAIN]                        = COMPONENT_DESC_EX(Brain, NULL, brain_fini, NULL),
  [CAMERA_TARGET_TAG]            = COMPONENT_DESC(CameraTargetTag),
  [CHARACTER_ANIMATOR_TAG]       = COMPONENT_DESC(CharacterAnimatorTag),
  [CHEST]                        = COMPONENT_DESC(ChestAttributes),
  [DESIRED_DIRECTION]            = COMPONENT_DESC(DesiredDirection),
  [DESTROYED_TAG]                = COMPONENT_DESC(DestroyedTag),
  [DIALOGUE]                     = { .size = sizeof(Dialogue) },
  [DOOR_ATTRIBUTES]              = COMPONENT_DESC(DoorAttributes),
  [DROP]                         = COMPONENT_DESC(Drop),
  [EMOJI]                        = COMPONENT_DESC(Emoji),
  [ENABLE_TILE_COLLISION_TAG]    = COMPONENT_DESC(EnableTileCollisionTag),
  [ENEMY_TAG]                    = COMPONENT_DESC(PlayerTag),
  [FOLLOWING_TARGET]             = COMPONENT_DESC(FollowingTarget),
  [HAND]                         = COMPONENT_DESC(Hand),
  [HAND_ANIMATION]               = COMPONENT_DESC(HandAnimation),
  [HEALTH]                       = COMPONENT_DESC(Health),
  [HEAL_BAR]                     = COMPONENT_DESC(HealthBar),
  [HITBOX]                       = COMPONENT_DESC_EX(HitBox, hitbox_init, NULL, NULL),
  [HOLDER]                       = COMPONENT_DESC_EX(Holder, holder_init, NULL, NULL),
  [INTELLIGENT_CHANGED]          = COMPONENT_DESC(IntelligentChanged),
  [INTERACTABLE]                 = { .size = sizeof(Interactable) },
  [INVULNERABLE]                 = COMPONENT_DESC(Invulnerable),
  [LADDER_ATTRIBUTES]            = COMPONENT_DESC(PortalAttributes),
  [LIFE_SPAN]                    = COMPONENT_DESC(LifeSpan),
  [MANA]                         = COMPONENT_DESC(Mana),
  [MERCHANT]                     = { .size = sizeof(Merchant) },
  [MOTION]                       = COMPONENT_DESC(Motion),
  [MOVE_SPEED]                   = COMPONENT_DESC(MoveSpeed),
  [NAME]                         = COMPONENT_DESC(Name),
  [PATH]                         = COMPONENT_DESC(Path),
  [PICKUPABLE_ATTRIBUTES]        = { .size = sizeof(PickupableAttributes) },
  [PLAYER_TAG]                   = COMPONENT_DESC(PlayerTag),
  [PROJECTILE_ATTRIBUTES]        = COMPONENT_DESC(ProjectileAttributes),
  [REMOVE_IF_OFFSCREEN]          = COMPONENT_DESC(RemoveIfOffScreen),
  [SCRIPT]                       = COMPONENT_DESC_EX(Script, NULL, script_fini, NULL),
  [STAGGER]                      = COMPONENT_DESC(Stagger),
  [STATS]                        = COMPONENT_DESC(Stats),
  [STATUS_EFFECT]                = COMPONENT_DESC(StatusEffect),
  [STRENGTH_CHANGED]             = COMPONENT_DESC(StrengthChanged),
  [TEXT]                         = COMPONENT_DESC(Text),
  [TRANSFORM]                    = COMPONENT_DESC_EX(Transform, transform_init, NULL, NULL),
  [UNABLE_TO_MOVE]               = COMPONENT_DESC(UnableToMove),
  [VISUAL]                       = COMPONENT_DESC_EX(Visual, visual_init, NULL, NULL),
  [VITALITY_CHANGED]             = COMPONENT_DESC(VitalityChanged),
  [WALK_DIRECTLY_TOWARD]         = COMPONENT_DESC(WalkDirectlyToward),
  [WEAPON_ATTRIBUTES]            = COMPONENT_DESC(WeaponAttributes),
  [WEAPON_CAST]                  = COMPONENT_DESC(WeaponCast),
  [WEAPON_CHARGE_ATTACK]         = COMPONENT_DESC(WeaponChargeAttack),
  [WEAPON_SHOOT]                 = COMPONENT_DESC(WeaponShoot),
  [WEAPON_SWING_ATTACK]          = COMPONENT_DESC(WeaponSwingAttack),
  [WEAPON_THUNDER_STORM_RELEASE] = COMPONENT_DESC(WeaponThunderStormRelease),
  [WEAPON_THUST_ATTACK]          = COMPONENT_DESC(WeaponThustAttack),
  [PATHFINDING_PARAMS]           = COMPONENT_DESC(PathfindingParams),
};

void brain_init(Brain* brain, bt_Tree* behaviorTree)
{
  brain->behaviorTree   = behaviorTree;
  brain->blackboard     = blackboard_create(bt_bbkey_set_count(bt_tree_key_set(behaviorTree)));
  brain->runtimeDataManager = bt_runtime_data_manager_create(bt_tree_node_count(behaviorTree));
}

void brain_fini(Brain* brain)
{
  blackboard_free(brain->blackboard);
  bt_runtime_data_manager_free(brain->runtimeDataManager);
}

void hitbox_init(HitBox* h)
{
  h->proxyId  = RTREE_NULL_NODE;
  h->anchor   = (Vec2){ 0 };
  h->category = 0;
  h->size     = (Vec2){ 16.f, 16.f };
  h->mask     = 0xffff;
}

void portal_attrs_init(PortalAttributes* attrs, const char* level, const char* dest, u16 direction)
{
  ASSERT(level != NULL && dest != NULL);
  SDL_strlcpy(attrs->level, level, LADDER_ATTRS_MAX_LEVEL_NAME_LEN);
  SDL_strlcpy(attrs->dest, dest, LADDER_ATTRS_MAX_DEST_LEN);
  attrs->exitDirection = direction;
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
  v->color  = (COLOR){ 255, 255, 255, 255 };
  v->anchor = (POINT){ 0 };
  v->sprite = (Sprite){ 0 };
  v->flip   = SDL_FLIP_NONE;
}

static void holder_init(Holder* holder)
{
  holder->value = ECS_NULL_ENT;
}

static void transform_init(Transform* t)
{
  t->lastPosition = t->position = (Vec2){ 0 };
  t->rotation                   = 0.0;
  t->z                          = 0.f;
}

static void aim_direction_init(AimDirection* fd)
{
  fd->value  = (Vec2){ 1.f, 0.f };
  fd->frezze = 0;
}

static void attack_target_init(AttackTarget* attack_target)
{
  attack_target->value = ECS_NULL_ENT;
}

void script_fini(Script* script)
{
  action_delete(script->action);
  script->action = NULL;
}

void interactable_init(Interactable* i, const char* const* cmds)
{
  i->numCommands = 0;
  while (*cmds && i->numCommands < INTERACTABLE_MAX_COMMANDS)
  {
    i->commands[i->numCommands++] = *cmds;
    ++cmds;
  }
}
