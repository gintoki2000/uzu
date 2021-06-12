#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "toolbox/toolbox.h"

ecs_entity_t make_golden_cross_hit_effect(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_static_pickupable_entity(ecs_Registry* ecs,
                                           u16           textureId,
                                           u16           itemType,
                                           Vec2          position,
                                           u8            quality);

typedef struct
{
  const char* name;
  const char* level;
  const char* dest;
  Vec2        position;
  Vec2        size;
  u16         direction;
} MakePortalParams;
ecs_entity_t make_portal(ecs_Registry* ecs, const MakePortalParams* params);
ecs_entity_t make_text_particle(ecs_Registry* ecs,
                                const char*   text,
                                Vec2          position,
                                Vec2          speed,
                                FONT*         font,
                                COLOR         color);

ecs_entity_t make_player(ecs_Registry* ecs, ecs_entity_t baseCharacter);

// weapons
ecs_entity_t make_spear(ecs_Registry* ecs);
ecs_entity_t make_cleaver(ecs_Registry* ecs);
ecs_entity_t make_golden_sword(ecs_Registry* ecs);
ecs_entity_t make_staff(ecs_Registry* ecs);
ecs_entity_t make_anime_sword(ecs_Registry* ecs);
ecs_entity_t make_bow(ecs_Registry* ecs);
ecs_entity_t make_katana(ecs_Registry* ecs);

// chars
ecs_entity_t make_wizzard(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_knight(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_elf(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_lizzard(ecs_Registry* ecs, Vec2 position);

// pickups
ecs_entity_t make_big_red_flask(ecs_Registry* ecs, Vec2 position, u8 quality);
ecs_entity_t make_red_flask(ecs_Registry* ecs, Vec2 position, u8 quality);
ecs_entity_t make_blue_flask(ecs_Registry* ecs, Vec2 position, u8 quality);
ecs_entity_t make_key_1_1(ecs_Registry* ecs, Vec2 position, u8 quality);
ecs_entity_t make_coin(ecs_Registry* ecs, Vec2 position, u8 quality);

// monsters
ecs_entity_t make_huge_demon(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_chort(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_wogol(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_imp(ecs_Registry* ecs, Vec2 position);

// misc
typedef struct MakeChestParams
{
  Vec2 position;
  Item items[CHEST_MAX_ITEMS];
  u16  numSlots;
  u16  state;
  u32  id;
} MakeChestParams;

typedef struct
{
  Vec2 position;
  u32  requiredKey;
} MakeDoorParams;

ecs_entity_t make_door(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_chest(ecs_Registry* ecs, const MakeChestParams* params);
ecs_entity_t make_trigger(ecs_Registry* ecs, Vec2 position, Vec2 size, u16 mask);

// effects
ecs_entity_t make_fx_blood_loss(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_fx_damage_indicator(ecs_Registry* ecs, Vec2 position, COLOR color, int amount);
ecs_entity_t make_fx_item_picked_up(ecs_Registry* ecs, Vec2 position, const char* itemName);
ecs_entity_t make_fx_cast_ice(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_fx_cast_fire(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_fx_ice_hit(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_fx_fire_hit(ecs_Registry* ecs, Vec2 position);
ecs_entity_t make_fx_slash(ecs_Registry* ecs, Vec2 position, double rot, SDL_RendererFlip flip);
ecs_entity_t make_fx_fire_bust(ecs_Registry* ecs, Vec2 position);

typedef struct MakeProjectileParams
{
  ecs_entity_t shooter;
  Vec2         position;
  Vec2         direction;
  u16          mask;
} MakeProjectileParams;
// projectiles
ecs_entity_t
make_fire_ball(ecs_Registry* ecs, ecs_entity_t shooter, Vec2 position, Vec2 direction, u16 mask);

ecs_entity_t
make_ice_arrow(ecs_Registry* ecs, ecs_entity_t shooter, Vec2 position, Vec2 direction, u16 mask);

ecs_entity_t
make_arrow(ecs_Registry* ecs, ecs_entity_t shooter, Vec2 position, Vec2 speed, u16 mask);

// npcs
ecs_entity_t make_npc_brian(ecs_Registry* ecs, Vec2 position, const char* conversation);

ecs_entity_t make_weapon(ecs_Registry* registry, u16 type);
ecs_entity_t make_character(ecs_Registry* registry, u16 job, Vec2 position);
#endif // ENTITY_FACTORY_H
