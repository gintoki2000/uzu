#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

typedef struct
{
  ecs_entity_t e1;
  ecs_entity_t e2;
} CollisionPair;

void collision_system_init(void);
void collision_system_fini(void);
void collision_system_update(void);
void collision_system_render_debug(void);
void collision_system_query(const AABB* aabb, Callback callback);
void collision_system_query_ex(const RECT*   rect,
                               u16           mask_bits,
                               ecs_entity_t* entities,
                               u16*          cnt,
                               u16           max);
#endif // COLLISION_SYSTEM_H
