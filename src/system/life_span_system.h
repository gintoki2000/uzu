#ifndef LIFE_SPAN_SYSTEM_H
#define LIFE_SPAN_SYSTEM_H
#include <toolbox/toolbox.h>
#include <ecs/ecs.h>

/**
 * [LifeSpan:in-out]
 * Tự động hủy một entity khi remmaing về 0
 *
 */
void LifeSpanSystem(Ecs* ecs);
#endif //LIFE_SPAN_SYSTEM_H
