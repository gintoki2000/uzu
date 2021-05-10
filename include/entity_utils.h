#ifndef UTILS
#define UTILS
#include "components.h"
#include <ecs/ecs.h>

ecs_entity_t scn_get_player(Ecs* ecs);
Vec2         snn_get_player_position(Ecs* ecs);
ecs_entity_t scn_find_portal(Ecs* ecs, const char* name);
ecs_entity_t scn_find(Ecs* ecs, const char* name);

void         ett_set_position(Ecs* ecs, ecs_entity_t entity, Vec2 pos);
Vec2         ett_get_position(Ecs* ecs, ecs_entity_t entity);
void         ett_equip_weapon(Ecs* ecs, ecs_entity_t entity, ecs_entity_t weapon);
void         ett_set_conversation(Ecs* ecs, ecs_entity_t entity, u16 conversation_id);
float        ett_dist(Ecs* ecs, ecs_entity_t e1, ecs_entity_t e2);
void         ett_set_hp(Ecs* ecs, ecs_entity_t entity, u16 hit_points);
void         ett_set_mp(Ecs* ecs, ecs_entity_t entity, u16 mana_points);
u16          ett_get_hp(Ecs* ecs, ecs_entity_t entity);
u16          ett_get_conversation(Ecs* ecs, ecs_entity_t entity);
const char*  ett_get_name(Ecs* ecs, ecs_entity_t entity);
Vec2         ett_get_velocity(Ecs* ecs, ecs_entity_t entity);
void         ett_set_velocity(Ecs* ecs, ecs_entity_t entity, Vec2 vel);
u16          ett_get_mp(Ecs* ecs, ecs_entity_t entity);
void         ett_attune_spell(Ecs* ecs, ecs_entity_t entity, u16 spell_id);
u16          ett_get_attuned_spell_type(Ecs* ecs, ecs_entity_t entity);
ecs_entity_t ett_get_equiped_weapon(Ecs* ecs, ecs_entity_t entity);
u16          ett_get_equiped_weapon_type(Ecs* ecs, ecs_entity_t holder);

typedef struct HandAnimParams
{
  const HandAnimKeyFrame* keyframes;
  BOOL                    realtiveCurrentState;
  Callback                cbFinished;
  Callback                cbFrame;
} HandAnimParams;

#define HAND_ANIM_PARAM_INIT                                                                       \
  {                                                                                                \
    .realtiveCurrentState = TRUE                                                                   \
  }

void         ett_animate_hand(Ecs* registry, ecs_entity_t entity, const HandAnimParams* params);
Vec2         ett_get_facing_direction(Ecs* registry, ecs_entity_t entity);
u16          ett_get_atk_mask(Ecs* registry, ecs_entity_t entity);
const Spell* ett_get_attuned_spell(Ecs* registry, ecs_entity_t entity);
void         ett_unable_to_move_push(Ecs* registry, ecs_entity_t entity);
void         ett_unable_to_move_pop(Ecs* registry, ecs_entity_t entity);
void         ett_apply_status_effect(Ecs* registry, ecs_entity_t entity, u16 type, u16 duration);

u16          wpn_get_type(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t wpn_get_holder(Ecs* registry, ecs_entity_t weapon);
#endif // UTILS
