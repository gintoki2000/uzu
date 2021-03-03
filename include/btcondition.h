#ifndef BT_CONDITION_H
#define BT_CONDITION_H
#include "behaviour_tree.h"
typedef struct BTCondition_HasAttackTarget BTCondition_HasAttackTarget;
typedef struct BTCondition_IsAttacked      BTCondition_IsAttacked;

BTCondition_HasAttackTarget* bt_condition_has_attack_target_new(BOOL invert_result);
BTCondition_IsAttacked*      bt_condition_is_attacked_new(BOOL invert_result);
#endif // BT_CONDITION_H
