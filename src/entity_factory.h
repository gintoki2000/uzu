#ifndef ENTITY_FACTORY_H
#define ENTITY_FACTORY_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

ecs_entity_t make_anime_sword(Ecs* ecs);
ecs_entity_t make_axe(Ecs* ecs);
ecs_entity_t make_knight(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t make_huge_demon(Ecs* ecs, ecs_entity_t weapon);
ecs_entity_t make_blood_stain_effect(Ecs* ecs, Vec2 pos);
ecs_entity_t make_bow(Ecs* ecs);
ecs_entity_t make_arrow(Ecs* ecs, Vec2 pos, Vec2 vel);
ecs_entity_t make_golden_sword(Ecs* ecs);
ecs_entity_t make_golden_cross_hit_effect(Ecs* ecs, Vec2 pos);

#endif //ENTITY_FACTORY_H
