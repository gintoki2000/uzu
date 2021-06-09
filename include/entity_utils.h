#ifndef UTILS
#define UTILS
#include "components.h"
#include <ecs/ecs.h>

ecs_entity_t scn_get_player(ecs_Registry* ecs);
Vec2         snn_get_player_position(ecs_Registry* ecs);
ecs_entity_t scn_find_portal(ecs_Registry* ecs, const char* name);
ecs_entity_t scn_find(ecs_Registry* ecs, const char* name);

void  ett_set_position(ecs_Registry* ecs, ecs_entity_t entity, Vec2 pos);
Vec2  ett_get_position(ecs_Registry* ecs, ecs_entity_t entity);
void  ett_equip_weapon(ecs_Registry* ecs, ecs_entity_t entity, ecs_entity_t weapon);
void  ett_set_conversation(ecs_Registry* ecs, ecs_entity_t entity, u16 conversation_id);
float ett_dist(ecs_Registry* ecs, ecs_entity_t e1, ecs_entity_t e2);
void  ett_set_hp(ecs_Registry* ecs, ecs_entity_t entity, u16 hit_points);
void  ett_set_mp(ecs_Registry* ecs, ecs_entity_t entity, u16 mana_points);
u16   ett_get_hp(ecs_Registry* ecs, ecs_entity_t entity);

const Conversation* ett_get_conversation(ecs_Registry* ecs, ecs_entity_t entity);
const char*         ett_get_name(ecs_Registry* ecs, ecs_entity_t entity);
Vec2                ett_get_velocity(ecs_Registry* ecs, ecs_entity_t entity);
void                ett_set_velocity(ecs_Registry* ecs, ecs_entity_t entity, Vec2 vel);
u16                 ett_get_mp(ecs_Registry* ecs, ecs_entity_t entity);
void                ett_attune_spell(ecs_Registry* ecs, ecs_entity_t entity, u16 spell_id);
u16                 ett_get_attuned_spell_type(ecs_Registry* ecs, ecs_entity_t entity);
ecs_entity_t        ett_get_equiped_weapon(ecs_Registry* ecs, ecs_entity_t entity);
u16                 ett_get_equiped_weapon_type(ecs_Registry* ecs, ecs_entity_t holder);

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

void ett_animate_hand(ecs_Registry* registry, ecs_entity_t entity, const HandAnimParams* params);
Vec2 ett_get_facing_direction(ecs_Registry* registry, ecs_entity_t entity);
u16  ett_get_atk_mask(ecs_Registry* registry, ecs_entity_t entity);
const Spell* ett_get_attuned_spell(ecs_Registry* registry, ecs_entity_t entity);
void         ett_unable_to_move_push(ecs_Registry* registry, ecs_entity_t entity);
void         ett_unable_to_move_pop(ecs_Registry* registry, ecs_entity_t entity);
void ett_apply_status_effect(ecs_Registry* registry, ecs_entity_t entity, u16 type, u16 duration);

u16          wpn_get_type(ecs_Registry* ecs, ecs_entity_t weapon);
ecs_entity_t wpn_get_holder(ecs_Registry* registry, ecs_entity_t weapon);
#endif // UTILS
