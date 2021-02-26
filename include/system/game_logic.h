#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include "ecs/ecs.h"

#define RUN_SYSTEM(sys) sys()

void effect_system_init(void);
void ai_system(void);
void character_controller_system(void);
void animator_system(void);
void input_blocking_system(void);
void camera_system(void);
void chest_system_init(void);
void collision_manager_system_init(void);
void dialogue_system_init(void);
void dialogue_system_fini(void);
void dialogue_system(void);
void damage_system_init(void);
void door_system_init(void);
void drop_system_init(void);
void equipment_system(void);
void following_system(void);
void health_system_init(void);
void health_system(void);
void interaction_system_init(void);
void interaction_system(void);
void knockback_system_init(void);
void late_destroying_system(void);
void life_span_system(void);
void merchant_system_init(void);
void merchant_system(void);
void motion_system(void);
void pickup_system_init(void);
void player_process_input(void*);
void player_controller_system_init(void);
void self_destruction_system(void);
void tile_collision_system(void);

void init_all_systems(void);
void shutdown_all_systems(void);

#endif // GAME_LOGIC_H
