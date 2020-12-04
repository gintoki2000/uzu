#ifndef LATE_DESTROYING_SYSTEM_H
#define LATE_DESTROYING_SYSTEM_H
#include <ecs/ecs.h>
/**
 * [TagToBeDestroyed: in]
 *
 * Hủy các entity được gắn componet "TagToBeDestroyed"
 */
void LateDestroyingSystem(Ecs* ecs);
#endif //LATE_DESTROYING_SYSTEM_H
