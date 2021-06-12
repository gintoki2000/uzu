#include "entity_factory.h"
#include "animations.h"
#include "behavior_tree.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "entity_utils.h"
#include "global.h"
#include "resources.h"

ecs_entity_t make_anime_sword(ecs_Registry* registry)
{

  ecs_entity_t entity;

  entity = ecs_create(registry);

  Visual*            visual;
  WeaponAttributes*  attributes;
  WeaponSwingAttack* sklswing;

  ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_ANIME_SWORD);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h + 4;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 2;
  attributes->typeId = WEAPON_ANIME_SWORD;

  sklswing       = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->code = 0;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

#define CHARACTER_SPRITE_WIDTH 16
#define CHARACTER_SPRITE_HEIGHT 28

typedef struct NewCharacterParams
{
  Vec2             position;
  const Animation* animations;
  u8               vitality;
  u8               intelligent;
  u8               strength;
  u8               agility;
} MakeCharacterParams;

ecs_entity_t make_character_base(ecs_Registry* registry, const MakeCharacterParams* params)
{
  ecs_entity_t entity;

  /*components */
  Transform* transform;
  Visual*    visual;
  Hand*      hand;
  HitBox*    hitbox;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28;

  hand                  = ecs_add(registry, entity, HAND);
  hand->weapon          = ECS_NULL_ENT;
  hand->originalPoint.x = 0;
  hand->originalPoint.y = -7;
  hand->length          = 6;

  ecs_add(registry, entity, DESIRED_DIRECTION);
  ecs_set(registry,
          entity,
          STATS,
          &(Stats){
              .vitality    = { .base = params->vitality },
              .intelligent = { .base = params->intelligent },
              .strength    = { .base = params->strength },
              .agility     = { .base = params->agility },
          });

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = params->animations });

  hitbox          = ecs_add(registry, entity, HITBOX);
  hitbox->size    = (Vec2){ 6.f, 10.f };
  hitbox->anchor  = (Vec2){ 3.f, 10.f };
  hitbox->proxyId = RTREE_NULL_NODE;

  ecs_add(registry, entity, MOTION);

  ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);

  ecs_add(registry, entity, AIM_DIRECTION);
  return entity;
}

ecs_entity_t make_knight(ecs_Registry* registry, Vec2 position)
{
  MakeCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_knight_m;
  params.vitality    = 10;
  params.intelligent = 1;
  params.strength    = 5;
  params.agility     = 10;
  return make_character_base(registry, &params);
}

ecs_entity_t make_elf(ecs_Registry* registry, Vec2 position)
{
  MakeCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_elf_m;
  params.vitality    = 10;
  params.intelligent = 3;
  params.strength    = 5;
  params.agility     = 10;
  return make_character_base(registry, &params);
}

ecs_entity_t make_wizzard(ecs_Registry* registry, Vec2 position)
{
  MakeCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_wizzard_m;
  params.vitality    = 3;
  params.intelligent = 5;
  params.strength    = 5;
  params.agility     = 10;
  return make_character_base(registry, &params);
}

ecs_entity_t make_lizzard(ecs_Registry* registry, Vec2 position)
{
  MakeCharacterParams params;
  params.position    = position;
  params.animations  = g_anims_lizzard_m;
  params.vitality    = 10;
  params.intelligent = 2;
  params.strength    = 5;
  params.agility     = 10;
  return make_character_base(registry, &params);
}

typedef struct MakeMonsterParams
{
  Vec2             position;
  const Animation* anims;
  Vec2             size;
  u8               vitality;
  u8               intelligent;
  u8               agility;
  u8               strength;
} MakeMonParams;

static const Vec2 _monBigSize   = { 20.f, 20.f };
static const Vec2 _monSizeSmall = { 10.f, 12.f };

ecs_entity_t make_monster_base(ecs_Registry* registry, const MakeMonParams* params)
{
  ecs_entity_t entity;

  /*components */
  Transform* transform;
  Visual*    visual;
  HitBox*    hitbox;

  const int sprite_width  = params->anims[0].spriteWidth;
  const int sprite_height = params->anims[0].spriteHeight;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  visual = ecs_add(registry, entity, VISUAL);

  visual->anchor.x = sprite_width / 2;
  visual->anchor.y = sprite_height;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = params->anims });

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = params->size;
  hitbox->anchor   = (Vec2){ params->size.x / 2.f, params->size.y };
  hitbox->mask     = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category = CATEGORY_ENEMY;

  ecs_add(registry, entity, ENEMY_TAG);

  ecs_set(registry,
          entity,
          HEAL_BAR,
          &(HealthBar){
              .len    = sprite_width,
              .anchor = { sprite_width / 2, sprite_height },
              .color  = { 0x42, 0xde, 0x23, 0xd0 },
          });

  ecs_set(registry,
          entity,
          STATS,
          &(Stats){
              .vitality    = { .base = params->vitality },
              .intelligent = { .base = params->intelligent },
              .strength    = { .base = params->strength },
              .agility     = { .base = params->agility },
          });

  ecs_add(registry, entity, MOTION);
  // ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);
  ecs_add(registry, entity, AIM_DIRECTION);
  ecs_set(registry, entity, ATTACK_MASK, &(AttackMask){ BIT(CATEGORY_PLAYER) });
  return entity;
}

ecs_entity_t make_huge_demon(ecs_Registry* registry, Vec2 position)
{
  MakeMonParams params = { 0 };
  params.agility       = 10;
  params.vitality      = 30;
  params.anims         = g_anims_huge_demon;
  params.position      = position;
  params.size          = _monBigSize;
  return make_monster_base(registry, &params);
}

ecs_entity_t make_imp(ecs_Registry* registry, Vec2 position)
{
  MakeMonParams params = { 0 };
  params.agility       = 10;
  params.vitality      = 6;
  params.anims         = g_anims_imp;
  params.position      = position;
  params.size          = _monSizeSmall;
  ecs_entity_t entity  = make_monster_base(registry, &params);
  return entity;
}

ecs_entity_t make_wogol(ecs_Registry* registry, Vec2 position)
{
  MakeMonParams params = { 0 };
  params.agility       = 10;
  params.vitality      = 6;
  params.anims         = g_anims_wogol;
  params.position      = position;
  params.size          = _monSizeSmall;
  ecs_entity_t entity  = make_monster_base(registry, &params);
  return entity;
}

struct AnimatedPickupableEntityParams
{
  u32              id;
  u8               quality;
  const Animation* animation;
  Vec2             position;
};
ecs_entity_t make_animated_pickupable_entity(ecs_Registry*                                registry,
                                             const struct AnimatedPickupableEntityParams* params)
{
  ecs_entity_t entity;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attributes;
  Animator*             animator;

  entity = ecs_create(registry);

  visual         = ecs_add(registry, entity, VISUAL);
  visual->anchor = (POINT){ 4, 4 };

  animator        = ecs_add(registry, entity, ANIMATOR);
  animator->anims = params->animation;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = (Vec2){ 8.f, 8.f };
  hitbox->anchor   = (Vec2){ 4.f, 4.f };
  hitbox->proxyId  = RTREE_NULL_NODE;
  hitbox->category = CATEGORY_ITEM;
  hitbox->mask     = BIT(CATEGORY_PLAYER);

  attributes          = ecs_add(registry, entity, PICKUPABLE_ATTRIBUTES);
  attributes->id      = params->id;
  attributes->sfx     = SFX_COIN;
  attributes->quality = params->quality;

  ecs_set(registry, entity, MOTION, &(Motion){ .friction = .2f, .bounching = .8f });
  return entity;
}

enum
{
  BB_CHORT_DESTINATION,
  BB_CHORT_NUM_ENTRIES
};

ecs_entity_t make_chort(ecs_Registry* registry, Vec2 position)
{
  MakeMonParams params = { 0 };
  params.agility       = 4;
  params.vitality      = 2;
  params.anims         = g_anims_chort;
  params.position      = position;
  params.size          = _monSizeSmall;
  ecs_entity_t entity  = make_monster_base(registry, &params);

  /*components */
  Hand*      hand;
  AggroArea* aggroArea;

  aggroArea           = ecs_add(registry, entity, AGGRO_AREA);
  aggroArea->radius   = TILE_SIZE * 7;
  aggroArea->position = position;

  ecs_set(registry,
          entity,
          DROP,
          &(Drop){
              .type    = { PICKUPABLE_RED_FLASK, PICKUPABLE_BLUE_FLASK, PICKUPABLE_COIN },
              .rate    = { 60, 60, 100 },
              .quality = { 1, 1, 1 },
              .cnt     = 3,
          });

  ecs_set(registry, entity, DESIRED_DIRECTION, &(DesiredDirection){ 0 });

  hand                  = ecs_add(registry, entity, HAND);
  hand->weapon          = ECS_NULL_ENT;
  hand->originalPoint.x = 0;
  hand->originalPoint.y = -7;
  hand->angle           = 40;

  static u16 weapons[] = { WEAPON_BOW, WEAPON_CLEAVER, WEAPON_SPEAR };

  ecs_entity_t weapon = gMakeWeaponFnTbl[weapons[rand() % 3]](registry);

  ett_equip_weapon(registry, entity, weapon);

  Name name;
  sprintf(name.value, "chort@%u", entity);
  ecs_set(registry, entity, NAME, &name);

  return entity;
}

ecs_entity_t make_axe(ecs_Registry* registry)
{
  ecs_entity_t axe;

  /*component*/
  Visual* visual;

  axe = ecs_create(registry);

  ecs_add(registry, axe, TRANSFORM);

  visual = ecs_add(registry, axe, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_AXE);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  return axe;
}

ecs_entity_t make_cleaver(ecs_Registry* registry)
{
  ecs_entity_t entity;

  entity = ecs_create(registry);

  Visual*            visual;
  WeaponAttributes*  attributes;
  WeaponSwingAttack* sklswing;

  ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_CLEAVER);
  visual->anchor.x = 4;
  visual->anchor.y = 4;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 2;
  attributes->typeId = WEAPON_CLEAVER;
  attributes->range  = (WeaponRange){ 10, 15 };

  sklswing       = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->code = 0;
  sklswing->wide = 32;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_katana(ecs_Registry* registry)
{

  ecs_entity_t entity;

  entity = ecs_create(registry);

  Visual*            visual;
  WeaponAttributes*  attributes;
  WeaponSwingAttack* sklswing;

  ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_KATANA);
  visual->anchor.x = 3;
  visual->anchor.y = 3;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 2;
  attributes->typeId = WEAPON_CLEAVER;
  attributes->range  = (WeaponRange){ 0, 32 };

  sklswing       = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->code = 0;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_golden_sword(ecs_Registry* registry)
{
  ecs_entity_t entity;

  entity = ecs_create(registry);

  Visual*            visual;
  WeaponAttributes*  attributes;
  WeaponSwingAttack* sklswing;

  ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_GOLDEN_SWORD);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 2;
  attributes->typeId = WEAPON_LAVIS_SWORD;

  sklswing       = ecs_add(registry, entity, WEAPON_SWING_ATTACK);
  sklswing->code = 0;

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t make_big_red_flask(ecs_Registry* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_ITEM_FLASK_RED_BIG,
                                       PICKUPABLE_BIG_RED_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t make_red_flask(ecs_Registry* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_ITEM_FLASK_RED,
                                       PICKUPABLE_RED_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t make_blue_flask(ecs_Registry* registry, Vec2 position, u8 quality)
{
  return make_static_pickupable_entity(registry,
                                       TEX_ITEM_BLUE_FLASK,
                                       PICKUPABLE_BLUE_FLASK,
                                       position,
                                       quality);
}

ecs_entity_t make_static_pickupable_entity(ecs_Registry* registry,
                                           u16           texture_id,
                                           u16           id,
                                           Vec2          position,
                                           u8            quality)
{
  ecs_entity_t entity;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attributes;

  entity = ecs_create(registry);

  visual                   = ecs_add(registry, entity, VISUAL);
  visual->sprite.textureId = texture_id;
  sprite_init(&visual->sprite, texture_id);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = (Vec2){ visual->sprite.rect.w, visual->sprite.rect.h };
  hitbox->anchor   = (Vec2){ hitbox->size.x / 2.f, hitbox->size.y / 2.f };
  hitbox->proxyId  = RTREE_NULL_NODE;
  hitbox->category = CATEGORY_ITEM;
  hitbox->mask     = BIT(CATEGORY_PLAYER);

  attributes          = ecs_add(registry, entity, PICKUPABLE_ATTRIBUTES);
  attributes->id      = id;
  attributes->sfx     = SFX_INTERACTION;
  attributes->quality = quality;

  ecs_set(registry, entity, MOTION, &(Motion){ .friction = .2f, .bounching = .8f });
  return entity;
}

ecs_entity_t make_player(ecs_Registry* registry, ecs_entity_t character)
{
  ecs_add(registry, character, PLAYER_TAG);
  ecs_add(registry, character, CAMERA_TARGET_TAG);

  HitBox* hitbox = ecs_get(registry, character, HITBOX);

  hitbox->category = CATEGORY_PLAYER;
  hitbox->mask     = BIT(CATEGORY_ITEM) | BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE) |
                 BIT(CATEGORY_LADDER) | BIT(CATEGORY_INTERACABLE);

  ecs_set(registry, character, ATTUNEMENT_SLOT, &(AttunementSlot){ SPELL_ID_NULL });

  ecs_set(registry,
          character,
          ATTACK_MASK,
          &(AttackMask){ BIT(CATEGORY_ENEMY) | BIT(CATEGORY_INTERACABLE) });

  return character;
}

ecs_entity_t
make_thunder(SDL_UNUSED ecs_Registry* registry, SDL_UNUSED Vec2 position, SDL_UNUSED u16 mask_bits)
{
  return ECS_NULL_ENT;
}

ecs_entity_t make_portal(ecs_Registry* registry, const MakePortalParams* params)
{
  ecs_entity_t entity;

  Transform* transform;
  HitBox*    hitbox;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = params->size;
  hitbox->mask     = BIT(CATEGORY_PLAYER);
  hitbox->category = CATEGORY_LADDER;
  hitbox->anchor.x = 0;
  hitbox->anchor.y = 0;

  portal_attrs_init(ecs_add(registry, entity, LADDER_ATTRIBUTES),
                    params->level,
                    params->dest,
                    params->direction);

  name_init(ecs_add(registry, entity, NAME), params->name);

  return entity;
}

ecs_entity_t make_text_particle(ecs_Registry* registry,
                                const char*   text,
                                Vec2          position,
                                Vec2          vel,
                                FONT*         font,
                                COLOR         color)
{
  ecs_entity_t entity;

  Transform* transform;
  Motion*    motion;
  LifeSpan*  life_span;

  entity = ecs_create(registry);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  motion      = ecs_add(registry, entity, MOTION);
  motion->vel = vel;

  text_init(ecs_add(registry, entity, TEXT), text, font, color);

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 30;

  return entity;
}

ecs_entity_t
make_fx_damage_indicator(ecs_Registry* registry, Vec2 position, COLOR color, int amount)
{
  char strnum[5];
  sprintf(strnum, "%d", amount);
  return make_text_particle(registry,
                            strnum,
                            position,
                            (Vec2){ 0.f, -30.f },
                            get_font(FONT_DAMAGE_INDICATOR),
                            color);
}

ecs_entity_t make_fx_item_picked_up(ecs_Registry* registry, Vec2 position, const char* item_name)
{
  return make_text_particle(registry,
                            item_name,
                            position,
                            (Vec2){ 0.f, -40.f },
                            get_font(FONT_ITEM_PICKED_UP),
                            (COLOR){ 122, 196, 10, 255 });
}

ecs_entity_t make_chest(ecs_Registry* registry, const MakeChestParams* params)
{
  ecs_entity_t entity;

  ChestAttributes* chest;
  Visual*          visual;
  Interactable*    interactable;
  HitBox*          hitbox;
  Transform*       transform;

  entity                   = ecs_create(registry);
  visual                   = ecs_add(registry, entity, VISUAL);
  visual->sprite.textureId = TEX_CHEST;
  visual->sprite.rect      = params->state == CHEST_STATE_CLOSE ? gRectChestClose : gRectChestClose;
  visual->anchor.x         = 8.f;
  visual->anchor.y         = 16.f;

  chest = ecs_add(registry, entity, CHEST);
  SDL_memcpy(chest->items, params->items, params->numSlots * sizeof(Item));
  chest->numSlots = params->numSlots;
  chest->id       = params->id;
  chest->state    = params->state;

  interactable              = ecs_add(registry, entity, INTERACTABLE);
  interactable->numCommands = 1;
  interactable->commands[0] = gCmdOpen;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = (Vec2){ 16, 16 };
  hitbox->anchor   = (Vec2){ 8.f, 16.f };
  hitbox->category = CATEGORY_INTERACABLE;
  hitbox->mask     = 0;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = params->position;

  return entity;
}

ecs_entity_t make_spear(ecs_Registry* registry)
{
  ecs_entity_t entity;

  Visual*            visual;
  WeaponThustAttack* thust;
  WeaponAttributes*  attributes;

  entity = ecs_create(registry);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_SPEAR);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = 3;

  ecs_add(registry, entity, TRANSFORM);

  thust       = ecs_add(registry, entity, WEAPON_THUST_ATTACK);
  thust->code = 0;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 1;
  attributes->range  = (WeaponRange){ 26, 32 };
  attributes->typeId = WEAPON_SPEAR;

  return entity;
}

ecs_entity_t make_door(ecs_Registry* registry, Vec2 position)
{
  ecs_entity_t entity;

  HitBox*         hitbox;
  Visual*         visual;
  Transform*      transform;
  DoorAttributes* attributes;
  Interactable*   interactable;

  entity = ecs_create(registry);

  visual                   = ecs_add(registry, entity, VISUAL);
  visual->sprite.textureId = TEX_DOOR;
  visual->sprite.rect      = gRectClose;
  visual->anchor.x         = 16;
  visual->anchor.y         = 34;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = (Vec2){ 32, 5 };
  hitbox->anchor   = (Vec2){ 16, 5 };
  hitbox->category = CATEGORY_INTERACABLE;
  hitbox->mask     = BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY);

  interactable              = ecs_add(registry, entity, INTERACTABLE);
  interactable->numCommands = 1;
  interactable->commands[0] = gCmdOpen;

  attributes              = ecs_add(registry, entity, DOOR_ATTRIBUTES);
  attributes->requiredKey = DOOR_NO_REQUIRED_KEY;
  attributes->state       = DOOR_STATE_CLOSE;

  return entity;
}

static const Animation* _bloodEffectTbl[] = {
  &g_anim_blood_loss_1,
  &g_anim_blood_loss_2,
};

#define NUM_BLOOD_LOSS_EFFECTS 2

ecs_entity_t make_fx_blood_loss(ecs_Registry* registry, Vec2 position)
{
  ecs_entity_t entity;
  Visual*      visual;
  Transform*   transform;
  LifeSpan*    life_span;
  Animator*    animator;

  const int sw = 100;
  const int sh = 100;
  int       i;

  i = rand() % NUM_BLOOD_LOSS_EFFECTS;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = _bloodEffectTbl[i] });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  life_span = ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 28 });

  return entity;
}

ecs_entity_t make_staff(ecs_Registry* registry)
{
  ecs_entity_t entity = ecs_create(registry);

  Visual*           visual;
  WeaponAttributes* attributes;

  ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, (TEX_WPN_RED_STAFF));
  visual->anchor.x = 9;
  visual->anchor.y = 4;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 1;
  attributes->typeId = WEAPON_STAFF;

  ecs_add(registry, entity, WEAPON_CAST);

  ecs_add(registry, entity, HOLDER);

  return entity;
}

ecs_entity_t
make_fire_ball(ecs_Registry* registry, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask)
{
  ecs_entity_t          entity = ecs_create(registry);
  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attributes;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = pos;
  transform->rotation = SDL_atan2(direction.y, direction.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = direction;
  motion->friction = 0.f;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size.x   = 26;
  hitbox->size.y   = 10;
  hitbox->anchor.x = 18;
  hitbox->anchor.y = 5;
  hitbox->category = CATEGORY_PROJECTILE;
  hitbox->mask     = mask;

  attributes                 = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attributes->damage         = 3;
  attributes->destroyWhenHit = TRUE;
  attributes->effect         = 0;
  attributes->damageType     = DAMAGE_TYPE_FIRE;
  attributes->impact         = TRUE;
  attributes->impactForce    = vec2_mul(vec2_unit(direction), 100.f);
  attributes->impactTime     = 16;
  attributes->sfx            = SFX_ID_NULL;
  attributes->shooter        = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_ball });

  return entity;
}

ecs_entity_t
make_ice_arrow(ecs_Registry* registry, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask)
{
  ecs_entity_t entity = ecs_create(registry);

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attributes;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;
  transform->rotation = SDL_atan2(speed.y, speed.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = speed;
  motion->friction = 0.f;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_arrow });

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size.x   = 32;
  hitbox->size.y   = 15;
  hitbox->anchor.x = 16;
  hitbox->anchor.y = 7;
  hitbox->category = CATEGORY_PROJECTILE;
  hitbox->mask     = mask;

  attributes                 = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attributes->damage         = 3;
  attributes->destroyWhenHit = TRUE;
  attributes->effect         = 0;
  attributes->damageType     = DAMAGE_TYPE_ICE;
  attributes->impact         = TRUE;
  attributes->impactForce    = vec2_mul(vec2_unit(speed), 100.f);
  attributes->impactTime     = 16;
  attributes->sfx            = SFX_ID_NULL;
  attributes->shooter        = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}

ecs_entity_t
make_arrow(ecs_Registry* registry, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask)
{
  ecs_entity_t entity = ecs_create(registry);

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  HitBox*               hitbox;
  ProjectileAttributes* attributes;

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 17;
  visual->anchor.y = 2;
  sprite_init(&visual->sprite, TEX_ARROW);

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;
  transform->rotation = SDL_atan2(speed.y, speed.x) * 57.2957795;

  motion           = ecs_add(registry, entity, MOTION);
  motion->vel      = speed;
  motion->friction = 0.f;

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size.x   = 24;
  hitbox->size.y   = 5;
  hitbox->anchor.x = 17;
  hitbox->anchor.y = 2;
  hitbox->category = CATEGORY_PROJECTILE;
  hitbox->mask     = mask;

  attributes                 = ecs_add(registry, entity, PROJECTILE_ATTRIBUTES);
  attributes->damage         = 2;
  attributes->destroyWhenHit = TRUE;
  attributes->effect         = 0;
  attributes->damageType     = DAMAGE_TYPE_THUST;
  attributes->impact         = TRUE;
  attributes->impactForce    = vec2_mul(vec2_unit(speed), 100.f);
  attributes->impactTime     = 16;
  attributes->sfx            = SFX_ID_NULL;
  attributes->shooter        = shooter;

  ecs_add(registry, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}

ecs_entity_t make_fx_cast_ice(ecs_Registry* registry, Vec2 pos)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = pos;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_cast });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 29 });
  return entity;
}

ecs_entity_t make_fx_cast_fire(ecs_Registry* registry, Vec2 position)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_cast });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 29 });
  return entity;
}

ecs_entity_t make_fx_slash(ecs_Registry* registry, Vec2 position, double rot, SDL_RendererFlip flip)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;

  const int sw = 48;
  const int sh = 48;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;
  visual->flip     = flip;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_slash });
  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;
  transform->rotation = rot;

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 10;

  return entity;
}

ecs_entity_t make_fx_fire_hit(ecs_Registry* registry, Vec2 position)
{

  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;

  const int sw = 64;
  const int sh = 64;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_fire_hit });
  ecs_set(registry, entity, LIFE_SPAN, &(LifeSpan){ 44 });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  return entity;
}
ecs_entity_t make_fx_ice_hit(ecs_Registry* registry, Vec2 position)
{
  ecs_entity_t entity;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;

  const int sw = 96;
  const int sh = 96;

  entity = ecs_create(registry);

  visual           = ecs_add(registry, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  ecs_set(registry, entity, ANIMATOR, &(Animator){ .anims = &g_anim_ice_hit });

  transform           = ecs_add(registry, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(registry, entity, LIFE_SPAN);
  life_span->remaining = 49;

  return entity;
}

ecs_entity_t make_npc_brian(ecs_Registry* registry, Vec2 position, const char* conversationName)
{
  ecs_entity_t        entity = ecs_create(registry);
  HitBox*             hitbox;
  const Conversation* con = get_conversation(conversationName);
  ASSERT(con != NULL);

  ecs_set(registry, entity, TRANSFORM, &(Transform){ .position = position });
  ecs_set(registry, entity, DIALOGUE, &(Dialogue){ con });
  ecs_set(registry, entity, NAME, &(Name){ .value = "brian" });
  ecs_set(registry,
          entity,
          ATTACK_MASK,
          &(AttackMask){ BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY) });
  /*
  ecs_set(registry,
          entity,
          HEAL_BAR,
          &(HealthBar){
              .color  = { 0x03, 0xb6, 0xfc, 0xff },
              .len    = 20,
              .anchor = { 6, 23 },
          });
  */
  ecs_set(registry, entity, MOTION, &(Motion){ 0 });

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->category = CATEGORY_INTERACABLE;
  hitbox->mask     = BIT(CATEGORY_PROJECTILE);
  hitbox->size.x   = 13;
  hitbox->size.y   = 19;
  hitbox->anchor.x = 6;
  hitbox->anchor.y = 19;

  interactable_init(ecs_add(registry, entity, INTERACTABLE), (const char*[]){ gCmdTalk, NULL });

  ecs_set(registry,
          entity,
          ANIMATOR,
          &(Animator){
              .anims = g_anims_elite_knight,
          });

  ecs_set(registry,
          entity,
          VISUAL,
          &(Visual){
              .anchor = { 16, 32 },
              .color  = { 0xff, 0xff, 0xff, 0xff },
          });

  ecs_set(registry,
          entity,
          HAND,
          &(Hand){
              .weapon = ECS_NULL_ENT,
          });
  ecs_add(registry, entity, DESIRED_DIRECTION);
  ecs_add(registry, entity, ENABLE_TILE_COLLISION_TAG);
  ecs_add(registry, entity, CHARACTER_ANIMATOR_TAG);
  return entity;
}

ecs_entity_t make_trigger(ecs_Registry* registry, Vec2 pos, Vec2 size, u16 mask)
{
  ecs_entity_t entity;

  HitBox* hitbox;

  entity = ecs_create(registry);

  hitbox           = ecs_add(registry, entity, HITBOX);
  hitbox->size     = size;
  hitbox->mask     = mask;
  hitbox->category = CATEGORY_TRIGGER;

  ecs_set(registry, entity, TRANSFORM, &(Transform){ .position = pos });

  return entity;
}

ecs_entity_t make_coin(ecs_Registry* registry, Vec2 position, u8 quality)
{
  return make_animated_pickupable_entity(registry,
                                         &(struct AnimatedPickupableEntityParams){
                                             .id        = PICKUPABLE_COIN,
                                             .quality   = quality,
                                             .position  = position,
                                             .animation = &g_anim_coin,
                                         });
}

ecs_entity_t make_key_1_1(ecs_Registry* registry, Vec2 pos, u8 quality)
{
  return make_static_pickupable_entity(registry, TEX_ITEM_KEY, PICKUPABLE_KEY_1_1, pos, quality);
}

ecs_entity_t make_bow(ecs_Registry* registry)
{
  ecs_entity_t entity;

  entity = ecs_create(registry);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attributes;

  transform = ecs_add(registry, entity, TRANSFORM);

  visual = ecs_add(registry, entity, VISUAL);
  sprite_init(&visual->sprite, TEX_WPN_BOW);
  visual->anchor.x = 0;
  visual->anchor.y = visual->sprite.rect.h / 2;

  attributes         = ecs_add(registry, entity, WEAPON_ATTRIBUTES);
  attributes->atk    = 2;
  attributes->typeId = WEAPON_BOW;
  attributes->range  = (WeaponRange){ 16 * 3, 16 * 5 };

  ecs_add(registry, entity, HOLDER);

  ecs_set(registry,
          entity,
          WEAPON_SHOOT,
          &(WeaponShoot){
              .triggerCode = 0,
              .projspd     = 170.f,
              .fire_rate   = 20,
          });

  return entity;
}

ecs_entity_t make_character(ecs_Registry* registry, u16 job, Vec2 position)
{
  ASSERT_MSG(job < NUM_JOBS, "Invalid job id");
  return gMakeCharacterFnTbl[job](registry, position);
}
