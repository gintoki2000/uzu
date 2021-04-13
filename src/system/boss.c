#include "components.h"
#include "ecs/ecs.h"
#include "system/event_messaging_sys.h"

extern Ecs* g_ecs;
static RECT _boss_room;

static ecs_entity_t get_boss_room(Ecs* registry)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  ecs_raw(registry, BOSS_ROOM, &entities, NULL, &cnt);
  return cnt > 0 ? entities[0] : ECS_NULL_ENT;
}

void on_level_loaded(SDL_UNUSED void* arg, MSG_LevelLoaded* msg)
{
}

void boss_system_init()
{
}
