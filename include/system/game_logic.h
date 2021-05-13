#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include "ecs/ecs.h"

#define RUN_SYSTEM(sys) sys()

void action_system(void);
void action_system_init(void);
void ai_system(void);
void animator_system(void);
void camera_system(void);
void character_controller_system(void);
void chest_system_init(void);
void collision_manager_system_init(void);
void damage_system_init(void);
void dialogue_system(void);
void dialogue_system_fini(void);
void dialogue_system_init(void);
void display_dialogue(u16 conversation_id, ecs_entity_t entity);
void door_system_init(void);
void drop_system_init(void);
void effect_system_init(void);
void emoji_system(void);
void equipment_system(void);
void following_system(void);
void health_system(void);
void health_system_init(void);
void interaction_system(void);
void interaction_system_init(void);
void knockback_system_init(void);
void late_destroying_system(void);
void life_span_system(void);
void merchant_system(void);
void merchant_system_init(void);
void motion_system(void);
void pickup_system_init(void);
void player_controller_system_init(void);
void player_process_input(void*);
void stagger_system_init(void);
void stats_system_init(void);
void stagger_system(void);
void stats_system(void);
void stats_system_init(void);
void tile_collision_system(void);
void update_attack_target_system(void);
void update_attack_target_system_init(void);
void update_facing_direction_system(void);
void status_effect_system(void);
void status_effect_rendering_system(void);
void status_effect_system_init(void);
void walk_directly_toward_system(void);
void pathfinding_system(void);
void pathfinding_system_init(void);

void draw_map_colliders(void);

void init_all_systems(void);
void shutdown_all_systems(void);

#endif // GAME_LOGIC_H
