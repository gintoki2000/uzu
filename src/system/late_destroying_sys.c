#include "late_destroying_sys.h"

#include <components.h>
#include <toolbox/toolbox.h>
#include <utils.h>

extern Ecs* g_ecs;
extern RECT g_viewport;

static void destroy_offscreen_entities()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  RemoveIfOffScreen* tags;
  Vec2               position;

  ecs_raw(g_ecs, REMOVE_IF_OFFSCREEN, &entities, (void**)&tags, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    position = get_entity_position(g_ecs, entities[i]);

    if (!SDL_PointInRect(&(POINT){position.x, position.y}, &g_viewport))
    {
      ecs_destroy(g_ecs, entities[i]);
      INFO("destory "ECS_ENT_FMT_PATTERN "\n", ECS_ENT_FMT_VARS(entities[i]));
    }
  }
}

static void destroy_tagged_entities()
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Equipment* equipment;

  ecs_raw(g_ecs, TAG_TO_BE_DESTROYED, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((equipment = ecs_get(g_ecs, entities[i], EQUIPMENT)))
    {
      ecs_destroy(g_ecs, equipment->weapon);
    }
    ecs_destroy(g_ecs, entities[i]);
  }
}

void late_destroying_system_update()
{
  destroy_offscreen_entities();
  destroy_tagged_entities();
}
