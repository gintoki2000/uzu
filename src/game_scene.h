#ifndef GAMESCR_H
#define GAMESCR_H
#include <ecs/ecs.h>
#include <toolbox/toolbox.h>

enum
{
  GAME_SCENE_SIG_BEGIN_LOAD_MAP,
  GAME_SCENE_SIG_CNT,
};

typedef struct
{
  Ecs* ecs;
} GameSceneEvt_BeginLoadMap;

extern Ecs* g_ecs;

void game_scene_connect_sig(int sig_id, slot_t slot, pointer_t arg);
#endif // GAMESCR_H
