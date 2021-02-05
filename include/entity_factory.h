#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "toolbox/toolbox.h"

ecs_entity_t make_axe(Ecs* ecs);
ecs_entity_t make_bow(Ecs* ecs);
ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel);
ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_pickupable_entity(Ecs* ecs, u16 texture_id, u16 item_type_id, Vec2 pos);
ecs_entity_t make_thunder_storm(Ecs* ecs, Vec2 center);
ecs_entity_t make_thunder(Ecs* ecs, Vec2 pos, u16 mask_bits);

typedef struct NewLadderParams
{
  const char* name;
  const char* level;
  const char* dest;
  Vec2        pos;
  Vec2        size;
} NewLadderParams;
ecs_entity_t make_ladder(Ecs* ecs, const NewLadderParams* params);
ecs_entity_t
make_text_particle(Ecs* ecs, const char* text, Vec2 pos, Vec2 vel, FONT* font, COLOR color);

ecs_entity_t make_player(Ecs* ecs, ecs_entity_t character, ecs_entity_t weapon);
ecs_entity_t make_npc(Ecs* ecs, ecs_entity_t character_base);
ecs_entity_t make_wizzard_npc(Ecs* ecs, Vec2 pos);

// weapons
ecs_entity_t make_spear(Ecs* ecs, u16 mask);
ecs_entity_t make_cleaver(Ecs* ecs, u16 mask);
ecs_entity_t make_golden_sword(Ecs* ecs, u16 mask);
ecs_entity_t make_staff(Ecs* ecs, u16 mask);
ecs_entity_t make_anime_sword(Ecs* ecs, u16 mask);

// chars
ecs_entity_t make_wizzard(Ecs* ecs, Vec2 pos);
ecs_entity_t make_knight(Ecs* ecs, Vec2 pos);
ecs_entity_t make_hunter(Ecs* ecs, Vec2 pos);
ecs_entity_t make_dragon(Ecs* ecs, Vec2 pos);

// pickups
ecs_entity_t make_big_red_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_red_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_blue_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_key_1_1(Ecs* ecs, Vec2 pos);
ecs_entity_t make_coin(Ecs* ecs, Vec2 pos);

// monsters
ecs_entity_t make_huge_demon(Ecs* ecs, Vec2 pos);
ecs_entity_t make_chort(Ecs* ecs, Vec2 pos);
ecs_entity_t make_wogol(Ecs* ecs, Vec2 pos);
ecs_entity_t make_imp(Ecs* ecs, Vec2 pos);

// other
ecs_entity_t make_door(Ecs* ecs, Vec2 pos);
ecs_entity_t make_chest(Ecs* ecs, Vec2 pos, Item items[CHEST_MAX_ITEMS], u16 cnt);
ecs_entity_t make_trigger(Ecs* ecs, Vec2 pos, Vec2 size, u16 mask);

// effects
ecs_entity_t make_blood_loss_particle(Ecs* ecs, Vec2 position);
ecs_entity_t make_damage_indicator_particle(Ecs* ecs, Vec2 pos, COLOR color, int amount);
ecs_entity_t make_item_picked_up_msg(Ecs* ecs, Vec2 pos, const char* item_name);
ecs_entity_t make_ice_cast_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_fire_cast_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_ice_hit_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_fire_hit_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_slash_effect(Ecs* ecs, Vec2 pos, SDL_RendererFlip flip);
ecs_entity_t make_fire_bust_effect(Ecs* ecs, Vec2 pos);

// projectiles
ecs_entity_t make_fire_ball(Ecs* ecs, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask);
ecs_entity_t make_ice_arrow(Ecs* ecs, ecs_entity_t shooter, Vec2 pos, Vec2 direction, u16 mask);

// npcs
ecs_entity_t make_npc_nova(Ecs* ecs, Vec2 position, u16 conversation_id);

#endif // ENTITY_FACTORY_H
