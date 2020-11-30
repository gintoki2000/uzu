#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include <ecs/ecs.h>

ecs_entity_t ettfactory_make_anime_sword(Ecs* ecs);
ecs_entity_t ettfactory_make_axe(Ecs* ecs);
ecs_entity_t ettfactory_make_knight(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t ettfactory_make_huge_demon(Ecs* ecs, ecs_entity_t weapon);

#endif //ENTITY_FACTORY_H
