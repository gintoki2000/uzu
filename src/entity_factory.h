#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include <ecs/ecs.h>

ecs_entity_t make_anime_sword(Ecs* ecs);
ecs_entity_t make_axe(Ecs* ecs);
ecs_entity_t make_knight(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t make_huge_demon(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t make_blood_stain_effect(Ecs* ecs, int x, int y);
ecs_entity_t make_bow(Ecs* ecs);

#endif //ENTITY_FACTORY_H
