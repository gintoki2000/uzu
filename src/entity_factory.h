#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>
#include "resources.h"

ecs_entity_t make_anime_sword(Ecs* ecs);
ecs_entity_t make_axe(Ecs* ecs);
ecs_entity_t make_cleaver(Ecs* ecs, u16 mask_bits);
ecs_entity_t make_knight(Ecs* ecs, Vec2 pos);
ecs_entity_t make_wizzard(Ecs* ecs, Vec2 pos);
ecs_entity_t make_huge_demon(Ecs* ecs);
ecs_entity_t make_chort(Ecs* ecs, Vec2 pos);
ecs_entity_t make_blood_stain_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_bow(Ecs* ecs);
ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel);
ecs_entity_t make_golden_sword(Ecs* ecs, u16 mask_bits);
ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_big_red_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_red_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_blue_flask(Ecs* ecs, Vec2 pos);
ecs_entity_t make_flask_base(Ecs* ecs, TextureId texture_id, ItemTypeId item_type_id, Vec2 pos);
ecs_entity_t make_thunder_storm(Ecs* ecs, Vec2 center);
ecs_entity_t make_thunder(Ecs* ecs, Vec2 pos, u16 mask_bits);
ecs_entity_t
make_ladder(Ecs* ecs, const char* name, Vec2 pos, Vec2 size, const char* level, const char* dest);
ecs_entity_t
make_text_particle(Ecs* ecs, const char* text, Vec2 pos, Vec2 vel, FONT* font, COLOR color);

ecs_entity_t make_make_damage_indicator_particle(Ecs* ecs, Vec2 pos, int amount);
ecs_entity_t make_item_picked_up_msg(Ecs* ecs, Vec2 pos, const char* item_name);

ecs_entity_t make_player(Ecs* ecs, ecs_entity_t character, ecs_entity_t weapon);
ecs_entity_t make_npc(Ecs* ecs, ecs_entity_t character_base);
ecs_entity_t make_wizzard_npc(Ecs* ecs, Vec2 pos);

#endif // ENTITY_FACTORY_H
