#include <ai/attack.h>
#include <ai/find_player_target.h>
#include <ai/find_random_destination.h>
#include <ai/follow.h>
#include <ai/is_player_cross_spot.h>
#include <ai/is_player_out_of_spot.h>
#include <ai/move_to.h>
#include <ai/set_dest_to_spot.h>
#include <ai/wait.h>
#include <behaviour_tree.h>
#include <components.h>
#include <constances.h>
#include <ecs/ecs.h>
#include <entity_factory.h>
#include <resources.h>
#include <utils.h>

ecs_entity_t make_anime_sword(Ecs* ecs)
{
  SDL_Texture* texture;
  ecs_entity_t sword;

  /*component*/
  Visual*    visual;
  Transform* transform;
  HitBox*    hitbox;

  texture = get_texture(TEX_ANIME_SWORD);

  sword = ecs_create(ecs);

  transform = ecs_add(ecs, sword, TRANSFORM);

  visual = ecs_add(ecs, sword, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  hitbox           = ecs_add(ecs, sword, HITBOX);
  hitbox->size     = VEC2(12.f, 30.f);
  hitbox->anchor   = VEC2(6.f, 30.f);
  hitbox->proxy_id = RTREE_NULL_NODE;
  hitbox->category = CATEGORY_WEAPON;

  return sword;
}

#define CHARACTER_SPRITE_WIDTH 16
#define CHARACTER_SPRITE_HEIGHT 28

ecs_entity_t make_character(Ecs* ecs, Vec2 pos, TextureId texture_id)
{
  ecs_entity_t entity;
  SDL_Texture* texture;
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

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 4;

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  visual = ecs_add(ecs, entity, VISUAL);

  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28;

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
  heath->hit_points     = 40;
  heath->max_hit_points = 40;

  ecs_add(ecs, entity, TILE_COLLISION_TAG);

  return entity;
}

ecs_entity_t make_knight(Ecs* ecs, Vec2 pos)
{
  return make_character(ecs, pos, TEX_KNIGHT);
}

ecs_entity_t make_wizzard(Ecs* ecs, Vec2 pos)
{
  return make_character(ecs, pos, TEX_WIZZARD);
}

ecs_entity_t make_huge_demon(Ecs* ecs)
{
  ecs_entity_t demon;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  HitBox*    hitbox;
  Health*    heath;
  HealthBar* healthBar;
  Motion*    motion;
  Drop*      drop;

  texture = get_texture(TEX_BIG_DEMON);
  animation_init(&anims[ANIM_STATE_HIT], texture, 32 * 6, 0, 1, 1, 32, 36);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, 32, 36);
  animation_init(&anims[ANIM_STATE_RUN], texture, 4 * 32, 0, 1, 4, 32, 36);

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 6;

  demon = ecs_create(ecs);

  transform = ecs_add(ecs, demon, TRANSFORM);

  visual = ecs_add(ecs, demon, VISUAL);

  visual->anchor.x = 32 / 2;
  visual->anchor.y = 36;

  animator = ecs_add(ecs, demon, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_RUN;
  animator->elapsed      = 0;

  hitbox            = ecs_add(ecs, demon, HITBOX);
  hitbox->size      = VEC2(20.f, 36.f);
  hitbox->anchor    = VEC2(10.f, 36.f);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->mask_bits = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category  = CATEGORY_ENEMY;

  ecs_add(ecs, demon, ENEMY_TAG);

  heath                 = ecs_add(ecs, demon, HEALTH);
  heath->max_hit_points = 100;
  heath->hit_points     = 60;

  healthBar         = ecs_add(ecs, demon, HEAL_BAR);
  healthBar->len    = 40;
  healthBar->anchor = (SDL_Point){ 20, 25 };

  motion            = ecs_add(ecs, demon, MOTION);
  motion->max_speed = 60.f;
  motion->max_force = 5.f;

  drop          = ecs_add(ecs, demon, DROP);
  drop->item1   = ITEM_TYPE_BIG_RED_FLASK;
  drop->item2   = ITEM_TYPE_RED_FLASK;
  drop->change1 = 70;
  drop->change2 = 40;

  ecs_add(ecs, demon, TILE_COLLISION_TAG);

  /*
  buid behaviour tree
  bt_Repeater*            root;
  bt_Sequence*            chase_seq;
  btt_FindRandomLocation* find_location_task;
  btt_MoveTo*             move_to_task;
  btt_Wait*               wait_task;

  find_location_task = btt_find_random_location_new();
  move_to_task       = btt_move_to_new();
  wait_task          = btt_wait_new(120);
  chase_seq           = bt_sequence_new();
  root               = bt_repeater_new((bt_Node*)chase_seq, -1);

  bt_sequence_add(chase_seq, (bt_Node*)find_location_task);
  bt_sequence_add(chase_seq, (bt_Node*)move_to_task);
  bt_sequence_add(chase_seq, (bt_Node*)wait_task);
  */

  return demon;
}

ecs_entity_t make_chort(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t entity;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform*  transform;
  Visual*     visual;
  Animator*   animator;
  HitBox*     hitbox;
  Health*     heath;
  HealthBar*  healthBar;
  Motion*     motion;
  Drop*       drop;
  AIAgent*    ai_agent;
  Equipment*  equipment;
  Controller* controller;
  Spot*       spot;

  texture = get_texture(TEX_CHORT);
  animation_init(&anims[ANIM_STATE_HIT], texture, 16 * 6, 0, 1, 1, 16, 24);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, 16, 24);
  animation_init(&anims[ANIM_STATE_RUN], texture, 16 * 4, 0, 1, 4, 16, 24);

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 6;

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  spot         = ecs_add(ecs, entity, SPOT);
  spot->radius = TILE_SIZE * 7;
  spot->pos    = pos;

  visual = ecs_add(ecs, entity, VISUAL);

  visual->anchor.x = 16 / 2;
  visual->anchor.y = 24;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(8.f, 14.f);
  hitbox->anchor    = VEC2(4.f, 14.f);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->mask_bits = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category  = CATEGORY_ENEMY;
  // hitbox->check_tile_collision = TRUE;

  ecs_add(ecs, entity, ENEMY_TAG);

  heath                 = ecs_add(ecs, entity, HEALTH);
  heath->max_hit_points = 10;
  heath->hit_points     = 10;

  healthBar         = ecs_add(ecs, entity, HEAL_BAR);
  healthBar->len    = 25;
  healthBar->anchor = (SDL_Point){ 20, 25 };

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 65;
  motion->max_force = 20;

  drop          = ecs_add(ecs, entity, DROP);
  drop->item1   = ITEM_TYPE_RED_FLASK;
  drop->item2   = ITEM_TYPE_BLUE_FLASK;
  drop->change1 = 70;
  drop->change2 = 60;

  ecs_add(ecs, entity, TILE_COLLISION_TAG);

  controller                = ecs_add(ecs, entity, CONTROLLER);
  controller->lock_movement = TRUE;

  BTRoot*            root;
  BTSequence*        chase_seq;
  MoveTo*            move_to;
  Wait *             wait, *wait_a_momment;
  BTSelector*        hostile;
  Attack*            attack;
  BTSequence*        attack_seq;
  FindPlayerTarget*  find_player_target;
  Follow*            follow;
  IsPlayerCrossSpot* is_player_cross_spot;
  BTSequence*        back_to_spot_seq;
  SetDestToSpot*     set_dest_to_spot;
  BTSelector*        selector;
  IsPlayerOutOfSpot* is_player_out_of_spot;

  root                  = bt_root_new();
  chase_seq             = bt_sequence_new();
  wait                  = wait_new(30);
  attack                = attack_new();
  hostile               = bt_selector_new();
  attack_seq            = bt_sequence_new();
  follow                = follow_new(15.f);
  find_player_target    = find_player_target_new();
  is_player_cross_spot  = is_player_cross_spot_new();
  move_to               = move_to_new(2.f);
  back_to_spot_seq      = bt_sequence_new();
  set_dest_to_spot      = set_dest_to_spot_new();
  selector              = bt_selector_new();
  is_player_out_of_spot = is_player_out_of_spot_new();
  wait_a_momment        = wait_new(90);

  bt_root_set_child(root, (BTNode*)selector);

  bt_decorator_set_child((BTDecorator*)is_player_cross_spot, (BTNode*)hostile);
  bt_decorator_set_child((BTDecorator*)is_player_out_of_spot, (BTNode*)back_to_spot_seq);

  bt_sequence_add(chase_seq, (BTNode*)find_player_target);
  bt_sequence_add(chase_seq, (BTNode*)follow);

  bt_sequence_add(attack_seq, (BTNode*)attack);
  bt_sequence_add(attack_seq, (BTNode*)wait);

  bt_sequence_add(back_to_spot_seq, (BTNode*)wait_a_momment);
  bt_sequence_add(back_to_spot_seq, (BTNode*)set_dest_to_spot);
  bt_sequence_add(back_to_spot_seq, (BTNode*)move_to);

  bt_selector_add(hostile, (BTNode*)chase_seq);
  bt_selector_add(hostile, (BTNode*)attack_seq);

  bt_selector_add(selector, (BTNode*)is_player_cross_spot);
  bt_selector_add(selector, (BTNode*)is_player_out_of_spot);

  ai_agent       = ecs_add(ecs, entity, AI_AGENT);
  ai_agent->root = (BTNode*)root;

  equipment                  = ecs_add(ecs, entity, EQUIPMENT);
  equipment->weapon          = ECS_NULL_ENT;
  equipment->weapon_anchor.x = 16 / 2;
  equipment->weapon_anchor.y = -6;

  ecs_entity_t weapon = make_cleaver(ecs, BIT(CATEGORY_PLAYER));

  equip(ecs, entity, weapon);

  return entity;
}

ecs_entity_t make_axe(Ecs* ecs)
{
  SDL_Texture* texture;
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

  SDL_Texture* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_CLEAVER);

  Transform*    transform;
  Visual*       visual;
  HitBox*       hitbox;
  WeaponCore*   core;
  DamageOutput* damage_output;
  wpskl_Swing*  wpskl_swing;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor    = VEC2(visual->anchor.x, visual->anchor.y);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_WEAPON;
  hitbox->mask_bits = mask_bits;

  core       = ecs_add(ecs, entity, WEAPON_CORE);
  core->atk  = 2;
  core->name = "cleaver";

  damage_output      = ecs_add(ecs, entity, DAMAGE_OUTPUT);
  damage_output->atk = 0;

  wpskl_swing            = ecs_add(ecs, entity, WEAPON_SKILL_SWING);
  wpskl_swing->is_active = FALSE;
  wpskl_swing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  wpskl_swing->step      = 0;
  wpskl_swing->timer     = 0;

  return entity;
}

ecs_entity_t make_blood_stain_effect(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t stain_effect;

  Visual*    visual;
  Animator*  animator;
  Transform* transform;
  LifeSpan*  life_span;

  Animation    animation;
  SDL_Texture* texture;
  texture = get_texture(TEX_BLOOD);
  animation_init(&animation, texture, 0, 0, 1, 4, 16, 16);

  stain_effect = ecs_create(ecs);

  visual         = ecs_add(ecs, stain_effect, VISUAL);
  visual->anchor = (SDL_Point){ 8, 8 };

  animator = ecs_add(ecs, stain_effect, ANIMATOR);
  animator_init(animator, &animation, 1);

  transform      = ecs_add(ecs, stain_effect, TRANSFORM);
  transform->pos = pos;

  life_span            = ecs_add(ecs, stain_effect, LIFE_SPAN);
  life_span->remaining = 15; // frames

  return stain_effect;
}

ecs_entity_t make_bow(Ecs* ecs)
{
  ecs_entity_t bow;

  Visual*    visual;
  Transform* transform;

  SDL_Texture* texture;
  texture = get_texture(TEX_BOW);

  bow = ecs_create(ecs);

  visual = ecs_add(ecs, bow, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h / 2;

  transform = ecs_add(ecs, bow, TRANSFORM);

  return bow;
}

ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel)
{
  SDL_Texture* texture;
  ecs_entity_t arrow;

  Transform*  transform;
  Visual*     visual;
  Motion*     motion;
  Projectile* projectile;
  HitBox*     hitbox;

  texture = get_texture(TEX_ARROW);

  arrow = ecs_create(ecs);

  transform      = ecs_add(ecs, arrow, TRANSFORM);
  transform->pos = pos;

  motion      = ecs_add(ecs, arrow, MOTION);
  motion->vel = vel;
  motion->acc = VEC2(0.f, 0.f);

  visual = ecs_add(ecs, arrow, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual_set_anchor_to_center(visual);

  projectile      = ecs_add(ecs, arrow, PROJECTILE);
  projectile->atk = 3;

  hitbox           = ecs_add(ecs, arrow, HITBOX);
  hitbox->size.x   = visual->sprite.rect.w;
  hitbox->size.y   = 3;
  hitbox->anchor.x = hitbox->size.x / 2.f;
  hitbox->anchor.y = hitbox->size.y / 2.f;
  hitbox->proxy_id = RTREE_NULL_NODE;
  hitbox->category = CATEGORY_PROJECTILE;

  return arrow;
}

ecs_entity_t make_golden_sword(Ecs* ecs, u16 mask_bits)
{
  ecs_entity_t entity;

  SDL_Texture* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_GOLDEN_SWORD);

  Transform*          transform;
  Visual*             visual;
  HitBox*             hitbox;
  WeaponCore*         core;
  DamageOutput*       damage_output;
  wpskl_Swing*        wpskl_swing;
  wpskl_ThunderStorm* wpskl_thunder_storm;

  transform = ecs_add(ecs, entity, TRANSFORM);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor    = VEC2(visual->anchor.x, visual->anchor.y);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_WEAPON;
  hitbox->mask_bits = mask_bits;

  core       = ecs_add(ecs, entity, WEAPON_CORE);
  core->atk  = 2;
  core->name = "golden sword";

  damage_output      = ecs_add(ecs, entity, DAMAGE_OUTPUT);
  damage_output->atk = 0;

  wpskl_swing            = ecs_add(ecs, entity, WEAPON_SKILL_SWING);
  wpskl_swing->is_active = FALSE;
  wpskl_swing->on_action = CHARACTER_ACTION_REGULAR_ATK;
  wpskl_swing->step      = 0;
  wpskl_swing->timer     = 0;

  /*
  wpskl_charge            = ecs_add(ecs, entity, WEAPON_SKILL_CHARGE);
  wpskl_charge->on_action = CHARACTER_ACTION_SPECIAL_ATK;
  */

  wpskl_thunder_storm            = ecs_add(ecs, entity, WEAPON_SKILL_THUNDER_STORM);
  wpskl_thunder_storm->on_action = CHARACTER_ACTION_SPECIAL_ATK;
  wpskl_thunder_storm->interval  = 10;
  wpskl_thunder_storm->total     = 5;

  return entity;
}

ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t entity;
  SDL_Texture* texture;
  Animation    animation;

  Transform* transform;
  Visual*    visual;
  Animator*  animator;
  LifeSpan*  life_span;

  texture = get_texture(TEX_GOLDEN_CROSS_HIT);
  animation_init(&animation, texture, 0, 0, 1, 10, 48, 48);

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  visual         = ecs_add(ecs, entity, VISUAL);
  visual->anchor = (SDL_Point){ 24, 24 };

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, &animation, 1);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = animation.sheet.count * animation.frame_duration;

  return entity;
}

ecs_entity_t make_big_red_flask(Ecs* ecs, Vec2 pos)
{
  return make_flask_base(ecs, TEX_FLASK_RED_BIG, ITEM_TYPE_BIG_RED_FLASK, pos);
}

ecs_entity_t make_red_flask(Ecs* ecs, Vec2 pos)
{
  return make_flask_base(ecs, TEX_FLASK_RED, ITEM_TYPE_RED_FLASK, pos);
}

ecs_entity_t make_blue_flask(Ecs* ecs, Vec2 pos)
{
  return make_flask_base(ecs, TEX_BLUE_FLASK, ITEM_TYPE_BLUE_FLASK, pos);
}

ecs_entity_t make_flask_base(Ecs* ecs, TextureId texture_id, ItemTypeId item_type_id, Vec2 pos)
{
  ecs_entity_t entity;
  SDL_Texture* texture;

  Visual*    visual;
  Transform* transform;
  HitBox*    hitbox;
  ItemTag*   tag;

  texture = get_texture(texture_id);

  entity = ecs_create(ecs);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual_set_anchor_to_center(visual);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(visual->sprite.rect.w, visual->sprite.rect.h);
  hitbox->anchor    = VEC2(hitbox->size.x / 2.f, hitbox->size.y / 2.f);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->category  = CATEGORY_ITEM;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);

  tag          = ecs_add(ecs, entity, ITEM_TAG);
  tag->item_id = item_type_id;
  return entity;
}

ecs_entity_t make_player(Ecs* ecs, ecs_entity_t character, ecs_entity_t weapon)
{
  ecs_add(ecs, character, PLAYER_TAG);
  ecs_add(ecs, character, CAMERA_TARGET_TAG);

  HitBox* hitbox = ecs_get(ecs, character, HITBOX);

  ASSERT(hitbox != NULL && "player entity must have hitbox component");

  equip(ecs, character, weapon);

  hitbox->category = CATEGORY_PLAYER;
  hitbox->mask_bits =
      BIT(CATEGORY_ITEM) | BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE) | BIT(CATEGORY_LADDER);

  return character;
}

ecs_entity_t make_thunder(Ecs* ecs, Vec2 pos, u16 mask_bits)
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

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

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

ecs_entity_t
make_ladder(Ecs* ecs, const char* name, Vec2 pos, Vec2 size, const char* level, const char* dest)
{
  ecs_entity_t entity;

  Transform* transform;
  HitBox*    hitbox;

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = size;
  hitbox->mask_bits = BIT(CATEGORY_PLAYER);
  hitbox->category  = CATEGORY_LADDER;

  level_switcher_init(ecs_add(ecs, entity, LEVEL_SWITCHER), level, dest);

  name_init(ecs_add(ecs, entity, NAME), name);

  return entity;
}

ecs_entity_t
make_text_particle(Ecs* ecs, const char* text, Vec2 pos, Vec2 vel, FONT* font, COLOR color)
{
  ecs_entity_t entity;

  Transform* transform;
  Motion*    motion;
  LifeSpan*  life_span;

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  motion            = ecs_add(ecs, entity, MOTION);
  motion->vel       = vel;
  motion->max_speed = 200.f;

  text_init(ecs_add(ecs, entity, TEXT), text, font, color);

  life_span            = ecs_add(ecs, entity, LIFE_SPAN);
  life_span->remaining = 30;

  return entity;
}

ecs_entity_t make_make_damage_indicator_particle(Ecs* ecs, Vec2 pos, int amount)
{
  char strnum[5];
  sprintf(strnum, "%d", amount);
  return make_text_particle(ecs,
                            strnum,
                            pos,
                            VEC2(0.f, -30.f),
                            get_font(FONT_DAMAGE_INDICATOR),
                            (COLOR){ 252, 78, 3, 255 });
}

ecs_entity_t make_item_picked_up_msg(Ecs* ecs, Vec2 pos, const char* item_name)
{
  return make_text_particle(ecs,
                            item_name,
                            pos,
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

  ItemPayload payloads[] = {
    { ITEM_TYPE_RED_FLASK, 10, 1 },  { ITEM_TYPE_BLUE_FLASK, MERCHANT_INIFINTE, 1 },
    { ITEM_TYPE_BLUE_FLASK, MERCHANT_INIFINTE, 1 }, { ITEM_TYPE_BLUE_FLASK, MERCHANT_INIFINTE, 1 },
    { ITEM_TYPE_BLUE_FLASK, MERCHANT_INIFINTE, 1 }, { ITEM_TYPE_BLUE_FLASK, MERCHANT_INIFINTE, 1 },
  };

  merchant->num_payloads = sizeof(payloads) / sizeof(ItemPayload);
  memcpy(merchant->payloads, payloads, sizeof(payloads));

  name_init(ecs_add(ecs, character_base, NAME), "luca");

  hitbox           = ecs_get(ecs, character_base, HITBOX);
  hitbox->category = CATEGORY_INTERACABLE;
  return character_base;
}

ecs_entity_t make_wizzard_npc(Ecs* ecs, Vec2 pos)
{
  return make_npc(ecs, make_wizzard(ecs, pos));
}
