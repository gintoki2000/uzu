#ifndef UTILS
#define UTILS
#include <ecs/ecs.h>
ecs_entity_t get_player(Ecs* ecs);
Vec2         get_player_position(Ecs* ecs);
BOOL         set_entity_position(Ecs* ecs, ecs_entity_t entity, Vec2 pos);
Vec2         get_entity_position(Ecs* ecs, ecs_entity_t entity);
ecs_entity_t find_ladder(Ecs* ecs, const char* name);
BOOL         equip(Ecs* ecs, ecs_entity_t entity, ecs_entity_t weapon);
ecs_entity_t find_entity(Ecs* ecs, const char* name);
BOOL         set_entity_conversation(Ecs* ecs, ecs_entity_t entity, u16 conversation_id);
BOOL         set_entity_velocity(Ecs* ecs, ecs_entity_t entity, Vec2 vel);
float        get_distance_between_two_entities(Ecs* ecs, ecs_entity_t e1, ecs_entity_t e2);
void         set_entity_hit_points(Ecs* ecs, ecs_entity_t entity, u16 hit_points);
void         set_entity_mana_points(Ecs* ecs, ecs_entity_t entity, u16 mana_points);
u16          get_entity_hit_points(Ecs* ecs, ecs_entity_t entity);
u16          get_entity_conversation(Ecs* ecs, ecs_entity_t entity);
const char*  get_entity_name(Ecs* ecs, ecs_entity_t entity);
Vec2         get_entity_velocity(Ecs* ecs, ecs_entity_t entity);
float        get_entity_vz(Ecs* ecs, ecs_entity_t entity);
u16          get_entity_mana_points(Ecs* ecs, ecs_entity_t entity);
void         set_spell(Ecs* ecs, ecs_entity_t entity, u16 spell_id);
u16          get_spell(Ecs* ecs, ecs_entity_t entity);
u16          get_weapon_type_id(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t get_equiped_weapon(Ecs* ecs, ecs_entity_t holder);
u16          get_equiped_weapon_type_id(Ecs* ecs, ecs_entity_t holder);

#endif // UTILS
