#ifndef GAMESCR_H
#define GAMESCR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>
extern Ecs* g_ecs;

void gamescr_connect_sig(int sig_id, slot_t slot, pointer_t arg);
#endif // GAMESCR_H
