#include "entity_factory.h"

#include "../include/entity_utils.h"
#include "ai/attack.h"
#include "ai/equip.c"
#include "ai/follow_attacker.h"
#include "ai/follow_player.h"
#include "ai/is_attacked.h"
#include "ai/is_player_cross_spot.h"
#include "ai/is_player_out_of_spot.h"
#include "ai/move_to.h"
#include "ai/set_dest_to_spot.h"
#include "ai/wait.h"
#include "behaviour_tree.h"
#include "components.h"
#include "constances.h"
#include "ecs/ecs.h"
#include "resources.h"

ecs_entity_t make_anime_sword(Ecs* ecs, u16 mask)
{

  ecs_entity_t entity;

  TEXTURE* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_ANIME_SWORD);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attributes;
  wpskl_Swing*      sklswing;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h + 4;

  attributes          = ecs_add(ecs, entity, WEAPON_ATTRIBUTES);
  attributes->atk     = 2;
  attributes->type_id = WEAPON_ANIME_SWORD;
  attributes->mask    = mask;

  sklswing            = ecs_add(ecs, entity, WEAPON_SKILL_SWING);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 40;

  ecs_add(ecs, entity, HOLDER);

  return entity;
}

#define CHARACTER_SPRITE_WIDTH 16
#define CHARACTER_SPRITE_HEIGHT 28

typedef struct NewCharacterParams
{
  Vec2 position;
  u16  texture_id;
  u16  hit_points;
} NewCharacterParams;

ecs_entity_t make_character_base(Ecs* ecs, Vec2 position, u16 texture_id)
{
  ecs_entity_t entity;
  TEXTURE*     texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform*  transform;
  Visual*     visual;
  Equipment*  equipment;
  Controller* controller;
  Animator*   animator;
  HitBox*     hitbox;
  Motion*     motion;
  Health*     heath;

  texture = get_texture(texture_id);
  animation_init(&anims[ANIM_STATE_HIT], texture, 0, 0, 1, 1, 16, 28);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 16 * 1, 0, 1, 4, 16, 28);
  animation_init(&anims[ANIM_STATE_RUN], texture, 16 * 5, 0, 1, 4, 16, 28);
  animation_init(&anims[ANIM_STATE_JUMP], texture, 16 * 6, 0, 1, 1, 16, 28);

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 4;

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28;
  // visual->color    = (COLOR){ 0xff, 0x00, 0x00, 0xff };

  equipment                  = ecs_add(ecs, entity, EQUIPMENT);
  equipment->weapon          = ECS_NULL_ENT;
  equipment->weapon_anchor.x = 16 / 2;
  equipment->weapon_anchor.y = -7;

  controller = ecs_add(ecs, entity, CONTROLLER);

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed      = 0;

  hitbox           = ecs_add(ecs, entity, HITBOX);
  hitbox->size     = VEC2(6.f, 10.f);
  hitbox->anchor   = VEC2(3.f, 10.f);
  hitbox->proxy_id = RTREE_NULL_NODE;

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 150;
  motion->max_force = 10;

  heath                 = ecs_add(ecs, entity, HEALTH);
  heath->hit_points     = 12;
  heath->max_hit_points = 12;

  ecs_add(ecs, entity, ENDABLE_TILE_COLLISION_TAG);
  ecs_add(ecs, entity, CHARACTER_ANIMATOR_TAG);

  return entity;
}

ecs_entity_t make_knight(Ecs* ecs, Vec2 position)
{
  return make_character_base(ecs, position, TEX_KNIGHT);
}

ecs_entity_t make_wizzard(Ecs* ecs, Vec2 position)
{
  return make_character_base(ecs, position, TEX_WIZZARD);
}

ecs_entity_t make_dragon(Ecs* ecs, Vec2 position)
{
  return make_character_base(ecs, position, TEX_LIZZARD);
}

typedef struct NewMonsterParams
{
  Vec2 position;
  u16  texture_id;
  u16  hit_points;
  Vec2 size;
} NewMonsterParams;

static const Vec2 s_big_size   = { 20.f, 20.f };
static const Vec2 s_small_size = { 10.f, 12.f };

ecs_entity_t make_monster_base(Ecs* ecs, const NewMonsterParams* params)
{
  ecs_entity_t entity;

  TEXTURE*  texture;
  Animation anims[NUM_ANIM_STATES];

  /*components */
  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  HitBox*    hitbox;
  Health*    heath;
  HealthBar* health_bar;
  Motion*    motion;

  int texture_width;
  int texture_height;
  int sprite_width;
  int sprite_height;

  texture = get_texture(params->texture_id);

  SDL_QueryTexture(texture, NULL, NULL, &texture_width, &texture_height);

  sprite_height = texture_height;
  sprite_width  = texture_width / 8;

  animation_init(&anims[ANIM_STATE_HIT],
                 texture,
                 6 * sprite_width,
                 0,
                 1,
                 1,
                 sprite_width,
                 sprite_height);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, sprite_width, sprite_height);
  animation_init(&anims[ANIM_STATE_RUN],
                 texture,
                 4 * sprite_width,
                 0,
                 1,
                 4,
                 sprite_width,
                 sprite_height);
  animation_init(&anims[ANIM_STATE_JUMP], texture, 0, 0, 1, 1, sprite_width, sprite_height);

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 6;

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = params->position;

  visual = ecs_add(ecs, entity, VISUAL);

  visual->anchor.x = sprite_width / 2;
  visual->anchor.y = sprite_height;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_RUN;
  animator->elapsed      = 0;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = params->size;
  hitbox->anchor    = VEC2(params->size.x / 2.f, params->size.y);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->mask_bits = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category  = CATEGORY_ENEMY;

  ecs_add(ecs, entity, ENEMY_TAG);

  heath                 = ecs_add(ecs, entity, HEALTH);
  heath->max_hit_points = params->hit_points;
  heath->hit_points     = params->hit_points;

  health_bar         = ecs_add(ecs, entity, HEAL_BAR);
  health_bar->len    = sprite_width;
  health_bar->anchor = (SDL_Point){ sprite_width / 2, sprite_height };

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 60.f;
  motion->max_force = 5.f;
  motion->friction  = 0.0f;

  ecs_add(ecs, entity, ENDABLE_TILE_COLLISION_TAG);
  ecs_add(ecs, entity, CHARACTER_ANIMATOR_TAG);
  return entity;
}

ecs_entity_t make_huge_demon(Ecs* ecs, Vec2 position)
{
  return make_monster_base(ecs, &(NewMonsterParams){ position, TEX_BIG_DEMON, 30, s_big_size });
}

ecs_entity_t make_imp(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity =
      make_monster_base(ecs, &(NewMonsterParams){ position, TEX_IMP, 5, s_small_size });
  ecs_add_w_data(ecs,
                 entity,
                 SELF_DESTRUCTION,
                 &(SelfDestruction){ .emiting_interval = 10, .range = 16, .target = ECS_NULL_ENT });

  BTRoot*                     root;
  BTCondition_IsPlayerInSpot* is_player_in_spot;
  BTTask_FollowPlayer*        follow_player;

  root              = bt_root_new();
  is_player_in_spot = bt_condition_is_player_in_spot_new();
  follow_player     = bt_task_follow_player_new(16);

  ecs_add(ecs, entity, CONTROLLER);

  bt_root_set_child(root, (BTNode*)is_player_in_spot);
  bt_decorator_set_child((BTDecorator*)is_player_in_spot, (BTNode*)follow_player);

  ecs_add_w_data(ecs, entity, BRAIN, &(Brain){ (BTNode*)root });
  ecs_add_w_data(ecs, entity, SPOT, &(Spot){ .position = position, .radius = 100.f });
  return entity;
}

ecs_entity_t make_wogol(Ecs* ecs, Vec2 position)
{
  return make_monster_base(ecs, &(NewMonsterParams){ position, TEX_WOGOL, 5, s_small_size });
}

ecs_entity_t make_chort(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity =
      make_monster_base(ecs, &(NewMonsterParams){ position, TEX_CHORT, 5, s_small_size });

  /*components */
  Drop*       drop;
  Brain*      brain;
  Equipment*  equipment;
  Controller* controller;
  Spot*       spot;

  spot           = ecs_add(ecs, entity, SPOT);
  spot->radius   = TILE_SIZE * 7;
  spot->position = position;

  ecs_add(ecs, entity, ENEMY_TAG);

  drop          = ecs_add(ecs, entity, DROP);
  drop->item1   = PICKUPABLE_RED_FLASK;
  drop->item2   = PICKUPABLE_BLUE_FLASK;
  drop->change1 = 70;
  drop->change2 = 60;

  controller = ecs_add(ecs, entity, CONTROLLER);

  BTRoot*                        root;
  BTTask_MoveTo*                 move_to;
  BTTask_Wait *                  wait, *wait_a_momment;
  BTSelector*                    hostile;
  BTTask_Attack*                 attack;
  BTSequence*                    attack_seq;
  BTCondition_IsPlayerInSpot*    is_player_cross_spot;
  BTSequence*                    back_to_spot_seq;
  BTTask_SetDestToSpot*          set_dest_to_spot;
  BTSelector*                    selector;
  BTCondition_IsPlayerOutOfSpot* is_player_out_of_spot;
  BTTask_FollowPlayer*           chase;

  root                  = bt_root_new();
  wait                  = bt_task_wait_new(30);
  attack                = bt_task_attack_new(BTTASK_ATTACK_MODE_PLAYER);
  hostile               = bt_selector_new();
  attack_seq            = bt_sequence_new();
  is_player_cross_spot  = bt_condition_is_player_in_spot_new();
  move_to               = bt_task_move_to_new(2.f);
  back_to_spot_seq      = bt_sequence_new();
  set_dest_to_spot      = bt_task_set_dest_to_spot_new();
  selector              = bt_selector_new();
  is_player_out_of_spot = bt_condition_is_player_out_of_spot_new();
  wait_a_momment        = bt_task_wait_new(50);
  chase                 = bt_task_follow_player_new(16.f);
  bt_root_set_child(root, (BTNode*)selector);

  bt_decorator_set_child((BTDecorator*)is_player_cross_spot, (BTNode*)hostile);
  bt_decorator_set_child((BTDecorator*)is_player_out_of_spot, (BTNode*)back_to_spot_seq);

  bt_sequence_add(attack_seq, (BTNode*)attack);
  bt_sequence_add(attack_seq, (BTNode*)wait);

  bt_sequence_add(back_to_spot_seq, (BTNode*)wait_a_momment);
  bt_sequence_add(back_to_spot_seq, (BTNode*)set_dest_to_spot);
  bt_sequence_add(back_to_spot_seq, (BTNode*)move_to);

  bt_selector_add(hostile, (BTNode*)chase);
  bt_selector_add(hostile, (BTNode*)attack_seq);

  bt_selector_add(selector, (BTNode*)is_player_cross_spot);
  bt_selector_add(selector, (BTNode*)is_player_out_of_spot);

  brain       = ecs_add(ecs, entity, BRAIN);
  brain->root = (BTNode*)root;

  equipment                  = ecs_add(ecs, entity, EQUIPMENT);
  equipment->weapon          = ECS_NULL_ENT;
  equipment->weapon_anchor.x = 16 / 2;
  equipment->weapon_anchor.y = -6;

  ecs_entity_t weapon = make_spear(ecs, BIT(CATEGORY_PLAYER));

  equip(ecs, entity, weapon);

  return entity;
}

ecs_entity_t make_axe(Ecs* ecs)
{
  TEXTURE*     texture;
  ecs_entity_t axe;

  /*component*/
  Visual*    visual;
  Transform* transform;

  texture = get_texture(TEX_AXE);

  axe = ecs_create(ecs);

  transform = ecs_add(ecs, axe, TRANSFORM);

  visual = ecs_add(ecs, axe, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  return axe;
}

ecs_entity_t make_cleaver(Ecs* ecs, u16 mask_bits)
{

  ecs_entity_t entity;

  TEXTURE* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_CLEAVER);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attributes;
  wpskl_Swing*      sklswing;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  attributes          = ecs_add(ecs, entity, WEAPON_ATTRIBUTES);
  attributes->atk     = 2;
  attributes->type_id = WEAPON_CLEAVER;
  attributes->mask    = mask_bits;

  sklswing            = ecs_add(ecs, entity, WEAPON_SKILL_SWING);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 20;

  ecs_add(ecs, entity, HOLDER);

  return entity;
}

ecs_entity_t make_blood_stain_effect(Ecs* ecs, Vec2 position)
{
  ecs_entity_t stain_effect;

  Visual*    visual;
  Animator*  animator;
  Transform* transform;
  LifeSpan*  life_span;

  Animation animation;
  TEXTURE*  texture;
  texture = get_texture(TEX_BLOOD);
  animation_init(&animation, texture, 0, 0, 1, 4, 16, 16);

  stain_effect = ecs_create(ecs);

  visual         = ecs_add(ecs, stain_effect, VISUAL);
  visual->anchor = (SDL_Point){ 8, 8 };

  animator = ecs_add(ecs, stain_effect, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, stain_effect, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, stain_effect, LIFE_SPAN);
  life_span->remaining = 15; // frames

  return stain_effect;
}

ecs_entity_t make_golden_sword(Ecs* ecs, u16 mask_bits)
{
  ecs_entity_t entity;

  TEXTURE* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_GOLDEN_SWORD);

  Transform*          transform;
  Visual*             visual;
  WeaponAttributes*   attributes;
  wpskl_Swing*        sklswing;
  wpskl_ThunderStorm* wpskl_thunder_storm;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  attributes          = ecs_add(ecs, entity, WEAPON_ATTRIBUTES);
  attributes->atk     = 2;
  attributes->type_id = WEAPON_LAVIS_SWORD;
  attributes->mask    = mask_bits;

  sklswing            = ecs_add(ecs, entity, WEAPON_SKILL_SWING);
  sklswing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  sklswing->range     = 100;

  wpskl_thunder_storm            = ecs_add(ecs, entity, WEAPON_SKILL_THUNDER_STORM);
  wpskl_thunder_storm->on_action = CHARACTER_ACTION_SPECIAL_ATK;
  wpskl_thunder_storm->interval  = 10;
  wpskl_thunder_storm->total     = 5;

  ecs_add(ecs, entity, HOLDER);

  return entity;
}

ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;
  TEXTURE*     texture;
  Animation    animation;

  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  LifeSpan*  life_span;

  texture = get_texture(TEX_GOLDEN_CROSS_HIT);
  animation_init(&animation, texture, 0, 0, 1, 10, 48, 48);

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  visual         = ecs_add(ecs, entity, VISUAL);
  visual->anchor = (SDL_Point){ 24, 24 };

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = animation.sheet.count * animation.frame_duration;

  return entity;
}

ecs_entity_t make_big_red_flask(Ecs* ecs, Vec2 position)
{
  return make_pickupable_entity(ecs, TEX_FLASK_RED_BIG, PICKUPABLE_BIG_RED_FLASK, position);
}

ecs_entity_t make_red_flask(Ecs* ecs, Vec2 position)
{
  return make_pickupable_entity(ecs, TEX_FLASK_RED, PICKUPABLE_RED_FLASK, position);
}

ecs_entity_t make_blue_flask(Ecs* ecs, Vec2 position)
{
  return make_pickupable_entity(ecs, TEX_BLUE_FLASK, PICKUPABLE_BLUE_FLASK, position);
}

ecs_entity_t make_pickupable_entity(Ecs* ecs, u16 texture_id, u16 id, Vec2 position)
{
  ecs_entity_t entity;
  TEXTURE*     texture;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attrs;

  texture = get_texture(texture_id);

  entity = ecs_create(ecs);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual_set_anchor_to_center(visual);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor    = VEC2(hitbox->size.x / 2.f, hitbox->size.y / 2.f);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_ITEM;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);

  attrs      = ecs_add(ecs, entity, PICKUPABLE_ATTRIBUTES);
  attrs->id  = id;
  attrs->sfx = SFX_INTERACTION;

  ecs_add_w_data(ecs,
                 entity,
                 MOTION,
                 &(Motion){ .friction = .2f, .max_speed = 150.f, .bounching = .8f });
  return entity;
}

ecs_entity_t make_player(Ecs* ecs, ecs_entity_t character, ecs_entity_t weapon)
{
  ecs_add(ecs, character, PLAYER_TAG);
  ecs_add(ecs, character, CAMERA_TARGET_TAG);

  AttunementSlot* attunement_slot;
  ManaPool*       mana_pool;

  HitBox* hitbox = ecs_get(ecs, character, HITBOX);

  ASSERT(hitbox != NULL && "player entity must have hitbox component");

  equip(ecs, character, weapon);

  hitbox->category  = CATEGORY_PLAYER;
  hitbox->mask_bits = BIT(CATEGORY_ITEM) | BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE) |
                      BIT(CATEGORY_LADDER) | BIT(CATEGORY_INTERACABLE);

  attunement_slot           = ecs_add(ecs, character, ATTUNEMENT_SLOT);
  attunement_slot->spell_id = SPELL_ICE_ARROW;

  mana_pool                  = ecs_add(ecs, character, MANA_POOL);
  mana_pool->mana_points     = 10;
  mana_pool->max_mana_points = 10;

  ecs_add_w_data(ecs,
                 character,
                 ATTACK_MASK,
                 &(AttackMask){ BIT(CATEGORY_ENEMY) | BIT(CATEGORY_INTERACABLE) });

  return character;
}

ecs_entity_t make_thunder(Ecs* ecs, Vec2 position, u16 mask_bits)
{
  ecs_entity_t entity;

  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  LifeSpan*  life_span;
  HitBox*    hitbox;

  Animation animation;
  animation_init(&animation, get_texture(TEX_YELLOW_THUNDER), 0, 0, 1, 8, 64, 256);
  animation.frame_duration = 4;

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  visual         = ecs_add(ecs, entity, VISUAL);
  visual->anchor = (SDL_Point){ 32, 224 };

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = (animation.sheet.count) * animation.frame_duration;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(16.f, 16.f);
  hitbox->anchor    = VEC2(8.f, 16.f);
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask_bits;
  hitbox->proxy_id  = RTREE_NULL_NODE;

  return entity;
}

ecs_entity_t make_ladder(Ecs* ecs, const NewLadderParams* params)
{
  ecs_entity_t entity;

  Transform* transform;
  HitBox*    hitbox;

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = params->pos;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = params->size;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);
  hitbox->category  = CATEGORY_LADDER;

  level_switcher_init(ecs_add(ecs, entity, LEVEL_SWITCHER), params->level, params->dest);

  name_init(ecs_add(ecs, entity, NAME), params->name);

  return entity;
}

ecs_entity_t
make_text_particle(Ecs* ecs, const char* text, Vec2 position, Vec2 vel, FONT* font, COLOR color)
{
  ecs_entity_t entity;

  Transform* transform;
  Motion*    motion;
  LifeSpan*  life_span;

  entity = ecs_create(ecs);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  motion            = ecs_add(ecs, entity, MOTION);
  motion->vel       = vel;
  motion->max_speed = 200.f;

  text_init(ecs_add(ecs, entity, TEXT), text, font, color);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 30;

  return entity;
}

ecs_entity_t make_damage_indicator_particle(Ecs* ecs, Vec2 position, COLOR color, int amount)
{
  char strnum[5];
  sprintf(strnum, "%d", amount);
  return make_text_particle(ecs,
                            strnum,
                            position,
                            VEC2(0.f, -30.f),
                            get_font(FONT_DAMAGE_INDICATOR),
                            color);
}

ecs_entity_t make_item_picked_up_msg(Ecs* ecs, Vec2 position, const char* item_name)
{
  return make_text_particle(ecs,
                            item_name,
                            position,
                            VEC2(0.f, -40.f),
                            get_font(FONT_ITEM_PICKED_UP),
                            (COLOR){ 122, 196, 10, 255 });
}

ecs_entity_t make_npc(Ecs* ecs, ecs_entity_t character_base)
{
  Interactable* interactable;
  HitBox*       hitbox;
  Dialogue*     dialogue;
  Merchant*     merchant;

  interactable = ecs_add(ecs, character_base, INTERACTABLE);

  const char* commands[] = { TEXT_COMMAND_TALK, TEXT_COMMAND_BUY };

  memcpy(interactable->commands, commands, sizeof(commands));
  interactable->num_commands = sizeof(commands) / sizeof(const char*);

  dialogue                  = ecs_add(ecs, character_base, DIALOGUE);
  dialogue->conversation_id = CONVERSATION_DEMO1;

  merchant = ecs_add(ecs, character_base, MERCHANT);

  ItemPayload payloads[] = { { ITEM_TYPE_RED_FLASK, 10, 1 },
                             { ITEM_TYPE_BIG_RED_FLASK, 5, 3 },
                             { ITEM_TYPE_BLUE_FLASK, 10, 2 } };

  merchant->num_payloads = sizeof(payloads) / sizeof(ItemPayload);
  memcpy(merchant->payloads, payloads, sizeof(payloads));

  name_init(ecs_add(ecs, character_base, NAME), "luca");

  hitbox           = ecs_get(ecs, character_base, HITBOX);
  hitbox->category = CATEGORY_INTERACABLE;
  return character_base;
}

ecs_entity_t make_wizzard_npc(Ecs* ecs, Vec2 position)
{
  return make_npc(ecs, make_wizzard(ecs, position));
}

ecs_entity_t make_chest(Ecs* ecs, Vec2 position, Item items[CHEST_MAX_ITEMS], u16 cnt)
{
  ecs_entity_t entity;

  Chest*        chest;
  Visual*       visual;
  Interactable* interactable;
  HitBox*       hitbox;
  Transform*    transform;

  entity              = ecs_create(ecs);
  visual              = ecs_add(ecs, entity, VISUAL);
  visual->sprite.tex  = get_texture(TEX_CHEST);
  visual->sprite.rect = RECT_CHEST_CLOSE;
  visual->anchor.x    = 8.f;
  visual->anchor.y    = 16.f;

  chest = ecs_add(ecs, entity, CHEST);
  memcpy(chest->items, items, cnt * sizeof(Item));
  chest->num_items = cnt;

  interactable               = ecs_add(ecs, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_OPEN;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = (Vec2){ 16, 16 };
  hitbox->anchor    = (Vec2){ 8.f, 16.f };
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = 0;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  return ECS_NULL_ENT;
}

ecs_entity_t make_spear(Ecs* ecs, u16 mask)
{
  ecs_entity_t entity;

  Visual*           visual;
  Transform*        transform;
  wpskl_Thust*      thust;
  WeaponAttributes* attributes;

  entity = ecs_create(ecs);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, get_texture(TEX_SPEAR));
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = 3;

  transform = ecs_add(ecs, entity, TRANSFORM);

  thust            = ecs_add(ecs, entity, WEAPON_SKILL_THUST);
  thust->on_action = CHARACTER_ACTION_REGULAR_ATK;

  attributes       = ecs_add(ecs, entity, WEAPON_ATTRIBUTES);
  attributes->atk  = 1;
  attributes->mask = mask;

  ecs_add(ecs, entity, HOLDER);

  return entity;
}

ecs_entity_t make_door(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;

  HitBox*       hitbox;
  Visual*       visual;
  Transform*    transform;
  DoorInfo*     door_info;
  Interactable* interactable;

  entity = ecs_create(ecs);

  visual              = ecs_add(ecs, entity, VISUAL);
  visual->sprite.tex  = get_texture(TEX_DOOR);
  visual->sprite.rect = RECT_DOOR_CLOSE;
  visual->anchor.x    = 16;
  visual->anchor.y    = 34;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = (Vec2){ 32, 5 };
  hitbox->anchor    = (Vec2){ 16, 5 };
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY);

  interactable               = ecs_add(ecs, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_OPEN;

  door_info               = ecs_add(ecs, entity, DOOR_INFO);
  door_info->required_key = ITEM_TYPE_KEY_1_1;
  door_info->state        = DOOR_STATE_CLOSE;

  return entity;
}

static const u16 kBloodParticleTexureTbl[] = {
  TEX_EFFECT_BLOOD_1,
  TEX_EFFECT_BLOOD_2,
};

#define kNumBloodParticleTextures 2

ecs_entity_t make_blood_loss_particle(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;

  Animation animation;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;
  Animator*  animator;

  const int sw = 100;
  const int sh = 100;
  int       i;

  i = rand() % kNumBloodParticleTextures;

  animation_init(&animation, get_texture(kBloodParticleTexureTbl[i]), 0, 0, 1, 28, sw, sh);

  entity = ecs_create(ecs);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 28;

  return entity;
}

ecs_entity_t make_staff(Ecs* ecs, u16 mask)
{
  ecs_entity_t entity = ecs_create(ecs);

  Transform*        transform;
  Visual*           visual;
  WeaponAttributes* attributes;
  Castable*         castable;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, get_texture(TEX_RED_STAFF));
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h / 2;

  attributes          = ecs_add(ecs, entity, WEAPON_ATTRIBUTES);
  attributes->atk     = 1;
  attributes->mask    = mask;
  attributes->type_id = WEAPON_STAFF;

  castable = ecs_add(ecs, entity, CASTABLE);

  ecs_add(ecs, entity, HOLDER);

  return entity;
}

ecs_entity_t make_fire_ball(Ecs* ecs, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask)
{
  ecs_entity_t entity = ecs_create(ecs);

  Animation animation;

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  Animator*             animator;
  HitBox*               hitbox;
  ProjectileAttributes* attributes;

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = pos;
  transform->rotation = SDL_atan2(direction.y, direction.x) * 57.2957795;

  motion            = ecs_add(ecs, entity, MOTION);
  motion->vel       = direction;
  motion->friction  = 0.f;
  motion->max_speed = 150.f;

  animation_init(&animation, get_texture(TEX_FIRE_BALL), 0, 0, 1, 10, 26, 10);
  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size.x    = 26;
  hitbox->size.y    = 10;
  hitbox->anchor.x  = 18;
  hitbox->anchor.y  = 5;
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask;

  attributes                   = ecs_add(ecs, entity, PROJECTILE_ATTRIBUTES);
  attributes->damage           = 3;
  attributes->destroy_when_hit = TRUE;
  attributes->effect           = 0;
  attributes->damage_type      = DAMAGE_TYPE_FIRE;
  attributes->impact           = TRUE;
  attributes->impact_force     = vec2_mul(vec2_unit_vec(direction), 100.f);
  attributes->impact_time      = 16;
  attributes->sfx              = SFX_ID_NULL;
  attributes->shooter          = shooter;

  ecs_add(ecs, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}

ecs_entity_t make_ice_arrow(Ecs* ecs, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask)
{
  ecs_entity_t entity = ecs_create(ecs);

  Animation animation;

  Visual*               visual;
  Transform*            transform;
  Motion*               motion;
  Animator*             animator;
  HitBox*               hitbox;
  ProjectileAttributes* attributes;

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 15;
  visual->anchor.y = 7;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = pos;
  transform->rotation = SDL_atan2(direction.y, direction.x) * 57.2957795;

  motion            = ecs_add(ecs, entity, MOTION);
  motion->vel       = direction;
  motion->friction  = 0.f;
  motion->max_speed = 150.f;

  animation_init(&animation, get_texture(TEX_ICE_ARROW), 0, 0, 1, 30, 32, 15);
  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size.x    = 32;
  hitbox->size.y    = 15;
  hitbox->anchor.x  = 16;
  hitbox->anchor.y  = 7;
  hitbox->category  = CATEGORY_PROJECTILE;
  hitbox->mask_bits = mask;

  attributes                   = ecs_add(ecs, entity, PROJECTILE_ATTRIBUTES);
  attributes->damage           = 3;
  attributes->destroy_when_hit = TRUE;
  attributes->effect           = 0;
  attributes->damage_type      = DAMAGE_TYPE_ICE;
  attributes->impact           = TRUE;
  attributes->impact_force     = vec2_mul(vec2_unit_vec(direction), 100.f);
  attributes->impact_time      = 16;
  attributes->sfx              = SFX_ID_NULL;
  attributes->shooter          = shooter;

  ecs_add(ecs, entity, REMOVE_IF_OFFSCREEN);

  return entity;
}
ecs_entity_t make_ice_cast_effect(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t entity;

  Animation anim;

  Visual*    visual;
  Transform* transform;
  Animator*  animator;
  LifeSpan*  life_span;

  entity = ecs_create(ecs);

  animation_init(&anim, get_texture(TEX_EFFECT_ICE_CAST), 0, 0, 1, 28, 96, 96);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = pos;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &anim, 1);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 28;

  return entity;
}

ecs_entity_t make_fire_cast_effect(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;

  Animation anim;

  Visual*    visual;
  Transform* transform;
  Animator*  animator;
  LifeSpan*  life_span;

  entity = ecs_create(ecs);

  animation_init(&anim, get_texture(TEX_EFFECT_FIRE_CAST), 0, 0, 1, 28, 96, 96);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 48;
  visual->anchor.y = 48;

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &anim, 1);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 28;

  return entity;
}

ecs_entity_t make_fire_bust_effect(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;

  Animation animation;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;
  Animator*  animator;

  const int sw = 64;
  const int sh = 64;

  animation_init(&animation, get_texture(TEX_EFFECT_FIRE_BUST), 0, 0, 1, 29, sw, sh);

  entity = ecs_create(ecs);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 29;

  return entity;
}

ecs_entity_t make_slash_effect(Ecs* ecs, Vec2 position, SDL_RendererFlip flip)
{
  ecs_entity_t entity;

  Animation animation;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;
  Animator*  animator;

  const int sw = 48;
  const int sh = 48;

  animation_init(&animation, get_texture(TEX_EFFECT_SLASH), 0, 0, 1, 10, sw, sh);

  entity = ecs_create(ecs);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;
  visual->flip     = flip;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 10;

  return entity;
}

ecs_entity_t make_fire_hit_effect(Ecs* ecs, Vec2 position)
{

  ecs_entity_t entity;

  Animation animation;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;
  Animator*  animator;

  const int sw = 64;
  const int sh = 64;

  animation_init(&animation, get_texture(TEX_EFFECT_FIRE_HIT), 0, 0, 1, 44, sw, sh);

  entity = ecs_create(ecs);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 44;

  return entity;
}
ecs_entity_t make_ice_hit_effect(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;

  Animation animation;

  Visual*    visual;
  Transform* transform;
  LifeSpan*  life_span;
  Animator*  animator;

  const int sw = 96;
  const int sh = 96;

  animation_init(&animation, get_texture(TEX_EFFECT_ICE_HIT), 0, 0, 1, 49, sw, sh);

  entity = ecs_create(ecs);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = sw / 2;
  visual->anchor.y = sh / 2;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 49;

  return entity;
}

ecs_entity_t make_npc_nova(Ecs* ecs, Vec2 position, u16 conversation_id)
{
  const int     sw     = 32;
  const int     sh     = 32;
  ecs_entity_t  entity = ecs_create(ecs);
  HitBox*       hitbox;
  Interactable* interactable;
  Animator*     animator;
  Visual*       visual;

  TEXTURE*  texture;
  Animation animations[NUM_ANIM_STATES];

  texture = get_texture(TEX_ELITE_KNIGHT);
  animation_init(&animations[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, sw, sh);
  animation_init(&animations[ANIM_STATE_RUN], texture, sw * 4, 0, 1, 4, sw, sh);
  animation_init(&animations[ANIM_STATE_HIT], texture, 0, 0, 1, 1, sw, sh);
  animation_init(&animations[ANIM_STATE_JUMP], texture, sw * 6, 0, 1, 1, sw, sh);

  animations[ANIM_STATE_IDLE].frame_duration = 16;
  animations[ANIM_STATE_RUN].frame_duration  = 3;

  ecs_add_w_data(ecs, entity, TRANSFORM, &(Transform){ .position = position });
  ecs_add_w_data(ecs, entity, DIALOGUE, &(Dialogue){ conversation_id });
  ecs_add_w_data(ecs, entity, NAME, &(Name){ .value = "nova" });
  ecs_add_w_data(ecs,
                 entity,
                 ATTACK_MASK,
                 &(AttackMask){ BIT(CATEGORY_PLAYER) | BIT(CATEGORY_ENEMY) });
  ecs_add_w_data(ecs, entity, HEALTH, &(Health){ 15, 15 });
  ecs_add_w_data(
      ecs,
      entity,
      HEAL_BAR,
      &(HealthBar){ .color = { 0x03, 0xb6, 0xfc, 0xff }, .len = 20, .anchor = { 6, 23 } });
  ecs_add_w_data(ecs, entity, DROP, &(Drop){ .item1 = PICKUPABLE_KEY_1_1, .change1 = 100, .change2 = 0 });
  ecs_add_w_data(ecs, entity, MOTION, &(Motion){ .max_speed = 60.f });
  ecs_add(ecs, entity, CHARACTER_ANIMATOR_TAG);

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->category  = CATEGORY_INTERACABLE;
  hitbox->mask_bits = BIT(CATEGORY_PROJECTILE);
  hitbox->size.x    = 13;
  hitbox->size.y    = 19;
  hitbox->anchor.x  = 6;
  hitbox->anchor.y  = 19;

  interactable               = ecs_add(ecs, entity, INTERACTABLE);
  interactable->num_commands = 1;
  interactable->commands[0]  = TEXT_COMMAND_TALK;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, animations, NUM_ANIM_STATES);

  visual           = ecs_add(ecs, entity, VISUAL);
  visual->anchor.x = 16;
  visual->anchor.y = 32;

  ecs_add(ecs, entity, CONTROLLER);

  BTRoot*                 root;
  BTTask_FollowAttacker*  follow_attacker;
  BTCondition_IsAttacked* is_attacked;
  BTTask_Attack*          attack;
  BTTask_Equip*           task_equip;
  BTSelector*             selector1;
  BTSelector*             selector2;
  BTSequence*             sequence;
  BTTask_Wait*            wait_affter_attack;

  root               = bt_root_new();
  follow_attacker    = bt_task_follow_attacker_new(16.f);
  is_attacked        = bt_condition_is_attacked_new();
  attack             = bt_task_attack_new(BTTASK_ATTACK_MODE_ATTACKER);
  task_equip         = bt_task_equip_new(WEAPON_ANIME_SWORD);
  selector1          = bt_selector_new();
  selector2          = bt_selector_new();
  wait_affter_attack = bt_task_wait_new(40);
  sequence           = bt_sequence_new();

  bt_root_set_child(root, (BTNode*)is_attacked);
  bt_decorator_set_child((BTDecorator*)is_attacked, (BTNode*)selector1);

  bt_selector_add(selector1, (BTNode*)follow_attacker);
  bt_selector_add(selector1, (BTNode*)selector2);

  bt_selector_add(selector2, (BTNode*)sequence);
  bt_selector_add(selector2, (BTNode*)task_equip);

  bt_sequence_add(sequence, (BTNode*)attack);
  bt_sequence_add(sequence, (BTNode*)wait_affter_attack);

  ecs_add_w_data(ecs, entity, BRAIN, &(Brain){ (BTNode*)root });

  ecs_add_w_data(ecs,
                 entity,
                 EQUIPMENT,
                 &(Equipment){ .weapon = ECS_NULL_ENT, .weapon_anchor = { 8, -9 } });

  ecs_add(ecs, entity, ENDABLE_TILE_COLLISION_TAG);
  return entity;
}

ecs_entity_t make_trigger(Ecs* ecs, Vec2 pos, Vec2 size, u16 mask)
{
  ecs_entity_t entity;

  HitBox* hitbox;

  entity = ecs_create(ecs);

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = size;
  hitbox->mask_bits = mask;
  hitbox->category  = CATEGORY_TRIGGER;

  ecs_add_w_data(ecs, entity, TRANSFORM, &(Transform){ .position = pos });

  return entity;
}

ecs_entity_t make_coin(Ecs* ecs, Vec2 position)
{
  ecs_entity_t entity;
  Animation    anim;

  Visual*               visual;
  Transform*            transform;
  HitBox*               hitbox;
  PickupableAttributes* attrs;
  Animator*             animator;

  animation_init(&anim, get_texture(TEX_COIN), 0, 0, 1, 4, 8, 8);
  anim.frame_duration = 6;

  entity = ecs_create(ecs);

  visual         = ecs_add(ecs, entity, VISUAL);
  visual->anchor = (POINT){ 4, 4 };

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &anim, 1);

  transform           = ecs_add(ecs, entity, TRANSFORM);
  transform->position = position;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = (Vec2){ 8.f, 8.f };
  hitbox->anchor    = (Vec2){ 4.f, 4.f };
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_ITEM;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);

  attrs      = ecs_add(ecs, entity, PICKUPABLE_ATTRIBUTES);
  attrs->id  = PICKUPABLE_COIN;
  attrs->sfx = SFX_COIN;
  attrs->coins = 5;

  ecs_add_w_data(ecs,
                 entity,
                 MOTION,
                 &(Motion){ .friction = .2f, .max_speed = 150.f, .bounching = .8f });
  return entity;
}

ecs_entity_t make_key_1_1(Ecs* ecs, Vec2 pos)
{
  return make_pickupable_entity(ecs, TEX_KEY, PICKUPABLE_KEY_1_1, pos);
}
