#ifndef ANIMATOR_SYSTEM_H
#define ANIMATOR_SYSTEM_H
#include <ecs/ecs.h>

void change_anim(Ecs* ecs, ecs_entity_t ett, int new_anim, SDL_bool reset);
void sys_animator_update(Ecs* ecs);
#endif //ANIMATOR_H
