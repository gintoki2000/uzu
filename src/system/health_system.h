#ifndef HEATH_SYSTEM_H
#define HEATH_SYSTEM_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>
enum
{
  SIG_ENTITY_DIED,
  NUM_HEALTH_SYSTEM_SIGS,
};

typedef struct EntityDiedEvent
{
  ecs_entity_t entity;
} EntityDiedEvent;

void health_system_init();
void health_system_fini();
void health_system_apply_damage(Ecs* ecs, ecs_entity_t entity, int amount);

#endif // HEATH_SYSTEM_H
