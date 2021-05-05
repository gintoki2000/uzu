#ifndef GAMESCR_H
#define GAMESCR_H
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

extern Ecs* g_ecs;
void game_scene_pause(void);
void game_scene_resume(void);
#endif // GAMESCR_H
