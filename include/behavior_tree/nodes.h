#ifndef BEHAVIOR_TREE_NODES_H
#define BEHAVIOR_TREE_NODES_H

#include "behavior_tree/common_enums.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"
typedef struct _BTCompositeNode BTCompositeNode;
typedef struct _BTDecorator     BTDecorator;
typedef struct _BTNode          BTNode;

BTCompositeNode* bt_selector_new();
BTCompositeNode* bt_sequence_new();

BTDecorator* bt_repeater_new(int times);
BTDecorator* bt_inverter_new(void);
BTDecorator* bt_componet_based_condition_new(ecs_entity_t   componentType,
                                             ComponentQuery componentQuery);
BTDecorator* bt_blackboard_based_condition_new(u32 bbKey, BBKeyQuery keyQuery);

BTNode* bt_attack_task_new(u32 fireCode);
BTNode* bt_walk_directly_toward_task_new(u32 destinationKey);
BTNode* bt_get_initial_position_task_new(u32 resultKey);
BTNode* bt_get_attack_target_location_task_new(u32 resultKey, float radius);
BTNode* bt_get_random_location_task_new(u32 resultKey, float radius);
BTNode* bt_wait_task_new(u32 duration);

#endif // BEHAVIOR_TREE_NODES_H
