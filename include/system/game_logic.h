#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H
#include "ecs/ecs.h"
void effect_system_init(void);
void ai_system_update(void);
void animator_controller_system_update(void);
void animator_system_update(void);
void input_blocking_system(void);
void camera_system_update(void);
void casting_system_update(void);
void casting_system_update(void);
void chest_system_init(void);
void collision_manager_system_init(void);
void dialogue_system_init(void);
void dialogue_system_fini(void);
void dialogue_system_update(void);
void damage_system_init(void);
void door_system_init(void);
void drop_system_init(void);
void equipment_system_update(void);
void following_system_update(void);
void health_system_init(void);
void health_system_update(void);
void interaction_system_init(void);
void interaction_system_update(void);
void knockback_system_init(void);
void late_destroying_system_update(void);
void life_span_system_update(void);
void merchant_system_init(void);
void merchant_system_update(void);
void motion_system_update(void);
void pickup_system_init(void);
void player_controller_system_update(void);
void player_controller_system_init(void);
void self_destruction_system (void);
void tile_collision_system_update(void);

void init_all_systems(void);
void shutdown_all_systems(void);

#define TIMER_TICK(__timer) ((__timer) > 0 && --(__timer) == 0)
#endif //GAME_LOGIC_H
