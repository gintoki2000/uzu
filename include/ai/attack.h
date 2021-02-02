#ifndef ATTACK_H
#define ATTACK_H
#include "behaviour_tree.h"
enum BTTask_Attack_Mode
{
	BTTASK_ATTACK_MODE_PLAYER,
	BTTASK_ATTACK_MODE_ATTACKER,
};
typedef struct BTTask_Attack BTTask_Attack;

BTTask_Attack* bt_task_attack_new(u16 mode);
#endif // ATTACK_H
