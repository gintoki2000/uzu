#include <ai/attack.h>
#include <ai/find_random_destination.h>
#include <ai/follow_player.h>
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
  TEXTURE*     texture;
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

ecs_entity_t make_character_base(Ecs* ecs, Vec2 pos, u16 texture_id)
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
  return make_character_base(ecs, pos, TEX_KNIGHT);
}

ecs_entity_t make_wizzard(Ecs* ecs, Vec2 pos)
{
  return make_character_base(ecs, pos, TEX_WIZZARD);
}

ecs_entity_t make_monster_base(Ecs* ecs, Vec2 pos, u16 texture_id)
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
  Drop*      drop;

  int texture_width;
  int texture_height;
  int sprite_width;
  int sprite_height;

  texture = get_texture(texture_id);

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

  anims[ANIM_STATE_IDLE].frame_duration = 8;
  anims[ANIM_STATE_RUN].frame_duration  = 6;

  entity = ecs_create(ecs);

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->pos = pos;

  visual = ecs_add(ecs, entity, VISUAL);

  visual->anchor.x = sprite_width / 2;
  visual->anchor.y = sprite_height;

  animator = ecs_add(ecs, entity, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_RUN;
  animator->elapsed      = 0;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size      = VEC2(sprite_width, sprite_height);
  hitbox->anchor    = VEC2(sprite_width / 2.f, sprite_height);
  hitbox->proxy_id  = RTREE_NULL_NODE;
  hitbox->mask_bits = BIT(CATEGORY_WEAPON) | BIT(CATEGORY_PROJECTILE);
  hitbox->category  = CATEGORY_ENEMY;

  ecs_add(ecs, entity, ENEMY_TAG);

  heath                 = ecs_add(ecs, entity, HEALTH);
  heath->max_hit_points = 100;
  heath->hit_points     = 100;

  health_bar         = ecs_add(ecs, entity, HEAL_BAR);
  health_bar->len    = sprite_width;
  health_bar->anchor = (SDL_Point){ sprite_width / 2, sprite_height };

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 60.f;
  motion->max_force = 5.f;

  drop          = ecs_add(ecs, entity, DROP);
  drop->item1   = ITEM_TYPE_BIG_RED_FLASK;
  drop->item2   = ITEM_TYPE_RED_FLASK;
  drop->change1 = 70;
  drop->change2 = 40;

  ecs_add(ecs, entity, TILE_COLLISION_TAG);
  return entity;
}

ecs_entity_t make_huge_demon(Ecs* ecs, Vec2 pos)
{
  return make_monster_base(ecs, pos, TEX_BIG_DEMON);
}

ecs_entity_t make_imp(Ecs* ecs, Vec2 pos)
{
  return make_monster_base(ecs, pos, TEX_IMP);
}

ecs_entity_t make_wogol(Ecs* ecs, Vec2 pos)
{
  return make_monster_base(ecs, pos, TEX_WOGOL);
}

ecs_entity_t make_chort(Ecs* ecs, Vec2 pos)
{
  ecs_entity_t entity;
  TEXTURE*     texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform*  transform;
  Visual*     visual;
  Animator*   animator;
  HitBox*     hitbox;
  Health*     heath;
  HealthBar*  health_bar;
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

  health_bar         = ecs_add(ecs, entity, HEAL_BAR);
  health_bar->len    = 25;
  health_bar->anchor = (SDL_Point){ 20, 25 };

  motion            = ecs_add(ecs, entity, MOTION);
  motion->max_speed = 65;
  motion->max_force = 20;

  drop          = ecs_add(ecs, entity, DROP);
  drop->item1   = ITEM_TYPE_RED_FLASK;
  drop->item2   = ITEM_TYPE_BLUE_FLASK;
  drop->change1 = 70;
  drop->change2 = 60;

  ecs_add(ecs, entity, TILE_COLLISION_TAG);

  controller = ecs_add(ecs, entity, CONTROLLER);

  BTRoot*              root;
  MoveTo*              move_to;
  Wait *               wait, *wait_a_momment;
  BTSelector*          hostile;
  Attack*              attack;
  BTSequence*          attack_seq;
  IsPlayerCrossSpot*   is_player_cross_spot;
  BTSequence*          back_to_spot_seq;
  SetDestToSpot*       set_dest_to_spot;
  BTSelector*          selector;
  IsPlayerOutOfSpot*   is_player_out_of_spot;
  BTTask_FollowPlayer* chase;

  root                  = bt_root_new();
  wait                  = wait_new(30);
  attack                = attack_new();
  hostile               = bt_selector_new();
  attack_seq            = bt_sequence_new();
  is_player_cross_spot  = is_player_cross_spot_new();
  move_to               = move_to_new(2.f);
  back_to_spot_seq      = bt_sequence_new();
  set_dest_to_spot      = set_dest_to_spot_new();
  selector              = bt_selector_new();
  is_player_out_of_spot = is_player_out_of_spot_new();
  wait_a_momment        = wait_new(50);
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

  Transform*    transform;
  Visual*       visual;
  HitBox*       hitbox;
  WeaponBase*   base;
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

  base       = ecs_add(ecs, entity, WEAPON_BASE);
  base->atk  = 2;
  base->name = "cleaver";

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

  Animation animation;
  TEXTURE*  texture;
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
  UNUSED(ecs);
  return ECS_NULL_ENT;
}

ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel)
{
  UNUSED(ecs);
  UNUSED(pos);
  UNUSED(vel);
  return ECS_NULL_ENT;
}

ecs_entity_t make_golden_sword(Ecs* ecs, u16 mask_bits)
{
  ecs_entity_t entity;

  TEXTURE* texture;

  entity  = ecs_create(ecs);
  texture = get_texture(TEX_GOLDEN_SWORD);

  Transform*          transform;
  Visual*             visual;
  HitBox*             hitbox;
  WeaponBase*         base;
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

  base          = ecs_add(ecs, entity, WEAPON_BASE);
  base->atk     = 2;
  base->name    = "lavis_sword";
  base->type_id = WEAPON_LAVIS_SWORD;

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
  TEXTURE*     texture;
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
  TEXTURE*     texture;

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

  ItemPayload payloads[] = { { ITEM_TYPE_RED_FLASK,    10, 1 },
                             { ITEM_TYPE_BIG_RED_FLASK, 5, 3 },
                             { ITEM_TYPE_BLUE_FLASK,   10, 2 } };

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

ecs_entity_t make_chest(Ecs* ecs, Vec2 pos)
{
  (void)ecs;
  (void)pos;
  return ECS_NULL_ENT;
}

ecs_entity_t make_spear(Ecs* ecs, u16 mask)
{
  ecs_entity_t entity;

  Visual*       visual;
  Transform*    transform;
  HitBox*       hitbox;
  DamageOutput* damage_output;
  wpskl_Thust*  thust;
  WeaponBase*   weapon_base;

  entity = ecs_create(ecs);

  visual = ecs_add(ecs, entity, VISUAL);
  sprite_init(&visual->sprite, get_texture(TEX_SPEAR));
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = 20;

  transform      = ecs_add(ecs, entity, TRANSFORM);
  transform->rot = 90;

  hitbox            = ecs_add(ecs, entity, HITBOX);
  hitbox->size.x    = 6;
  hitbox->size.y    = 30;
  hitbox->anchor.x  = 3;
  hitbox->category  = CATEGORY_WEAPON;
  hitbox->anchor.y  = 20;
  hitbox->mask_bits = mask;
  hitbox->proxy_id  = RTREE_NULL_NODE;

  damage_output = ecs_add(ecs, entity, DAMAGE_OUTPUT);

  thust            = ecs_add(ecs, entity, WEAPON_SKILL_THUST);
  thust->on_action = CHARACTER_ACTION_REGULAR_ATK;

  weapon_base         = ecs_add(ecs, entity, WEAPON_BASE);
  weapon_base->wearer = ECS_NULL_ENT;

  return entity;
}
