#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "toolbox/toolbox.h"

ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 position);
ecs_entity_t
make_static_pickupable_entity(Ecs* ecs, u16 textureId, u16 itemType, Vec2 position, u8 quality);

typedef struct
{
  const char* name;
  const char* level;
  const char* dest;
  Vec2        position;
  Vec2        size;
  u16         direction;
} MakePortalParams;
ecs_entity_t make_portal(Ecs* ecs, const MakePortalParams* params);
ecs_entity_t
make_text_particle(Ecs* ecs, const char* text, Vec2 position, Vec2 speed, FONT* font, COLOR color);

ecs_entity_t make_player(Ecs* ecs, ecs_entity_t baseCharacter);

// weapons
ecs_entity_t make_spear(Ecs* ecs);
ecs_entity_t make_cleaver(Ecs* ecs);
ecs_entity_t make_golden_sword(Ecs* ecs);
ecs_entity_t make_staff(Ecs* ecs);
ecs_entity_t make_anime_sword(Ecs* ecs);
ecs_entity_t make_bow(Ecs* ecs);
ecs_entity_t make_katana(Ecs* ecs);

// chars
ecs_entity_t make_wizzard(Ecs* ecs, Vec2 position);
ecs_entity_t make_knight(Ecs* ecs, Vec2 position);
ecs_entity_t make_elf(Ecs* ecs, Vec2 position);
ecs_entity_t make_lizzard(Ecs* ecs, Vec2 position);

// pickups
ecs_entity_t make_big_red_flask(Ecs* ecs, Vec2 position, u8 quality);
ecs_entity_t make_red_flask(Ecs* ecs, Vec2 position, u8 quality);
ecs_entity_t make_blue_flask(Ecs* ecs, Vec2 position, u8 quality);
ecs_entity_t make_key_1_1(Ecs* ecs, Vec2 position, u8 quality);
ecs_entity_t make_coin(Ecs* ecs, Vec2 position, u8 quality);

// monsters
ecs_entity_t make_huge_demon(Ecs* ecs, Vec2 position);
ecs_entity_t make_chort(Ecs* ecs, Vec2 position);
ecs_entity_t make_wogol(Ecs* ecs, Vec2 position);
ecs_entity_t make_imp(Ecs* ecs, Vec2 position);

// misc
typedef struct MakeChestParams
{
  Vec2 position;
  Item items[CHEST_MAX_ITEMS];
  u16  num_slots;
  u16  state;
  u32  id;
} MakeChestParams;

typedef struct
{
  Vec2 position;
  u16  required_key;
} MakeDoorParams;

ecs_entity_t make_door(Ecs* ecs, Vec2 position);
ecs_entity_t make_chest(Ecs* ecs, const MakeChestParams* params);
ecs_entity_t make_trigger(Ecs* ecs, Vec2 position, Vec2 size, u16 mask);

// effects
ecs_entity_t make_fx_blood_loss(Ecs* ecs, Vec2 position);
ecs_entity_t make_fx_damage_indicator(Ecs* ecs, Vec2 position, COLOR color, int amount);
ecs_entity_t make_fx_item_picked_up(Ecs* ecs, Vec2 position, const char* item_name);
ecs_entity_t make_fx_cast_ice(Ecs* ecs, Vec2 position);
ecs_entity_t make_fx_cast_fire(Ecs* ecs, Vec2 position);
ecs_entity_t make_fx_ice_hit(Ecs* ecs, Vec2 position);
ecs_entity_t make_fx_fire_hit(Ecs* ecs, Vec2 position);
ecs_entity_t make_fx_slash(Ecs* ecs, Vec2 position, double rot, SDL_RendererFlip flip);
ecs_entity_t make_fx_fire_bust(Ecs* ecs, Vec2 position);

typedef struct MakeProjectileParams
{
  ecs_entity_t shooter;
  Vec2         position;
  Vec2         direction;
  u16          mask;
} MakeProjectileParams;
// projectiles
ecs_entity_t
make_fire_ball(Ecs* ecs, ecs_entity_t shooter, Vec2 position, Vec2 direction, u16 mask);

ecs_entity_t
make_ice_arrow(Ecs* ecs, ecs_entity_t shooter, Vec2 position, Vec2 direction, u16 mask);

ecs_entity_t make_arrow(Ecs* ecs, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask);

// npcs
ecs_entity_t make_npc_brian(Ecs* ecs, Vec2 position, u16 conversation_id);

ecs_entity_t make_weapon(Ecs* registry, u16 type);
ecs_entity_t make_character(Ecs* registry, u16 job, Vec2 position);
#endif // ENTITY_FACTORY_H
