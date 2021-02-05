#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} CollisionPair;

void collision_system_init(void);
void collision_system_fini(void);
void collision_system_update(void);
void collision_system_render_debug(void);
void collision_box_query(const RECT* rect, u16 mask_bits, Callback callback);
#endif // COLLISION_SYSTEM_H
