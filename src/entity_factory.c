#include <ai/attack.h>
#include <ai/find_player_target.h>
#include <ai/find_random_destination.h>
#include <ai/follow.h>
#include <ai/is_player_cross_spot.h>
#include <ai/move_to.h>
#include <ai/set_dest_to_spot.h>
#include <ai/wait.h>
#include <behaviour_tree.h>
#include <components.h>
#include <constances.h>
#include <ecs/ecs.h>
#include <entity_factory.h>
#include <resources.h>

static BOOL equip(Ecs* ecs, ecs_entity_t entity, ecs_entity_t weapon)
{
  Equipment*  equipment;
  WeaponCore* weapon_core;
  equipment   = ecs_get(ecs, entity, EQUIPMENT);
  weapon_core = ecs_get(ecs, weapon, WEAPON_CORE);

  if (weapon_core == NULL)
    return FALSE;
  equipment->weapon   = weapon;
  weapon_core->wearer = entity;

  return TRUE;
}

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

ecs_entity_t make_knight(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t knight;
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
  Heath*      heath;

  texture = get_texture(TEX_KNIGHT);
  animation_init(&anims[ANIM_STATE_HIT], texture, 0, 0, 1, 1, 16, 28);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 16 * 1, 0, 1, 4, 16, 28);
  animation_init(&anims[ANIM_STATE_RUN], texture, 16 * 5, 0, 1, 4, 16, 28);

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 4;

  knight = ecs_create(ecs);

  transform      = ecs_add(ecs, knight, TRANSFORM);
  transform->pos = pos;

  INFO("pos=(%2.f, %2.f)", pos.x, pos.y);

  visual = ecs_add(ecs, knight, VISUAL);

  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28;

  equipment                  = ecs_add(ecs, knight, EQUIPMENT);
  equipment->weapon          = ECS_NULL_ENT;
  equipment->weapon_anchor.x = 16 / 2;
  equipment->weapon_anchor.y = -7;

  controller = ecs_add(ecs, knight, CONTROLLER);

  animator = ecs_add(ecs, knight, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed      = 0;

  hitbox           = ecs_add(ecs, knight, HITBOX);
  hitbox->size     = VEC2(6.f, 10.f);
  hitbox->anchor   = VEC2(3.f, 10.f);
  hitbox->proxy_id = RTREE_NULL_NODE;

  motion            = ecs_add(ecs, knight, MOTION);
  motion->max_speed = 150;
  motion->max_force = 10;

  heath                 = ecs_add(ecs, knight, HEATH);
  heath->hit_points     = 40;
  heath->max_hit_points = 40;

  ecs_add(ecs, knight, TILE_COLLISION_TAG);

  return knight;
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
  Heath*     heath;
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

  heath                 = ecs_add(ecs, demon, HEATH);
  heath->max_hit_points = 100;
  heath->hit_points     = 60;

  healthBar         = ecs_add(ecs, demon, HEAL_BAR);
  healthBar->len    = 40;
  healthBar->anchor = (SDL_Point){ 20, 25 };

  motion            = ecs_add(ecs, demon, MOTION);
  motion->max_speed = 60.f;
  motion->max_force = 5.f;

  drop          = ecs_add(ecs, demon, DROP);
  drop->item1   = ITEM_BIG_RED_FLASK;
  drop->item2   = ITEM_RED_FLASK;
  drop->change1 = 30;
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
  Heath*      heath;
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

  heath                 = ecs_add(ecs, entity, HEATH);
  heath->max_hit_points = 10;
  heath->hit_points     = 10;

  healthBar         = ecs_add(ecs, entity, HEAL_BAR);
  healthBar->len    = 10;
  healthBar->anchor = (SDL_Point){ 20, 25 };

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 65;
  motion->max_force = 20;

  drop          = ecs_add(ecs, entity, DROP);
  drop->item1   = ITEM_BIG_RED_FLASK;
  drop->item2   = ITEM_RED_FLASK;
  drop->change1 = 30;
  drop->change2 = 40;

  ecs_add(ecs, entity, TILE_COLLISION_TAG);

  controller                = ecs_add(ecs, entity, CONTROLLER);
  controller->lock_movement = TRUE;

  bt_Root*     root;
  bt_Sequence* chase_seq;
  // FindRandomDestination* find_random_destination;
  MoveTo*            move_to;
  Wait *             wait, *wait2;
  bt_Selector*       hostile;
  Attack*            attack;
  bt_Sequence*       attack_seq;
  FindPlayerTarget*  find_player_target;
  Follow*            follow;
  IsPlayerCrossSpot* is_player_cross_spot;
  bt_Sequence*       back_to_spot_seq;
  SetDestToSpot*     set_dest_to_spot;
  bt_Selector*       selector;

  root      = bt_root_new();
  chase_seq = bt_sequence_new();
  // find_random_destination = find_random_destination_new();
  // move_to                 = move_to_new(TILE_SIZE * 2, 1.f);
  wait                 = wait_new(30);
  wait2                = wait_new(60);
  attack               = attack_new();
  hostile              = bt_selector_new();
  attack_seq           = bt_sequence_new();
  follow               = follow_new(15.f);
  find_player_target   = find_player_target_new();
  is_player_cross_spot = is_player_cross_spot_new();
  move_to              = move_to_new(2.f);
  back_to_spot_seq     = bt_sequence_new();
  set_dest_to_spot     = set_dest_to_spot_new();
  selector             = bt_selector_new();

  bt_root_set_child(root, (bt_Node*)selector);

  bt_decorator_set_child((bt_Decorator*)is_player_cross_spot, (bt_Node*)hostile);

  // bt_decorator_set_child((bt_Decorator*)is_player_far_away, (bt_Node*)chase_seq);

  bt_sequence_add(chase_seq, (bt_Node*)find_player_target);
  bt_sequence_add(chase_seq, (bt_Node*)follow);

  bt_sequence_add(attack_seq, (bt_Node*)attack);
  bt_sequence_add(attack_seq, (bt_Node*)wait);

  bt_sequence_add(back_to_spot_seq, (bt_Node*)set_dest_to_spot);
  bt_sequence_add(back_to_spot_seq, (bt_Node*)move_to);

  bt_selector_add(hostile, (bt_Node*)chase_seq);
  bt_selector_add(hostile, (bt_Node*)attack_seq);

  bt_selector_add(selector, (bt_Node*)is_player_cross_spot);
  bt_selector_add(selector, (bt_Node*)back_to_spot_seq);

  ai_agent       = ecs_add(ecs, entity, AI_AGENT);
  ai_agent->root = (bt_Node*)root;

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
  ecs_entity_t entity;
  SDL_Texture* texture;

  Visual*    visual;
  Transform* transform;
  HitBox*    hitbox;
  ItemTag*   tag;

  texture = get_texture(TEX_FLASK_RED_BIG);

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
  tag->item_id = ITEM_BIG_RED_FLASK;

  return entity;
}

ecs_entity_t make_red_flask(Ecs* ecs, Vec2 pos)
{

  ecs_entity_t entity;
  SDL_Texture* texture;

  Visual*    visual;
  Transform* transform;
  HitBox*    hitbox;
  ItemTag*   tag;

  texture = get_texture(TEX_FLASK_RED);

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
  tag->item_id = ITEM_RED_FLASK;
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
