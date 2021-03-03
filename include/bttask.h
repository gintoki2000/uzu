#ifndef BT_TASK_H
#define BT_TASK_H
#include "behaviour_tree.h"

typedef struct BTTask_SetDestToSpot             BTTask_SetDestToSpot;
typedef struct BTTask_Wait                      BTTask_Wait;
typedef struct BTTask_MoveTo                    BTTask_MoveTo;
typedef struct BTTask_Attack                    BTTask_Attack;
typedef struct BTTask_SetAttackTargetToAttacker BTTask_SetAttackTargetToAttacker;
typedef struct BTTask_Equip                     BTTask_Equip;

BTTask_Wait*                      bt_task_wait_new(int total_ticks);
BTTask_MoveTo*                    bt_task_move_to_new(float arrive_radius);
BTTask_Attack*                    bt_task_attack_new(void);
BTTask_SetAttackTargetToAttacker* bt_task_set_attack_target_to_attacker_new(void);
BTTask_SetDestToSpot*             bt_task_set_dest_to_spot_new();
BTTask_Equip*                     bt_task_equip_new(u16 weapon_id);
#endif // BT_TASK_H
