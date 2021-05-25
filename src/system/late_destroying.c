#include "components.h"
#include "entity_utils.h"
#include "system/game_logic.h"
#include "toolbox/toolbox.h"

extern Ecs* gEcs;
extern RECT gViewport;

static void destroy_offscreen_entities()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Vec2 position;

  ecs_raw(gEcs, REMOVE_IF_OFFSCREEN, &entities, NULL, &cnt);

  for (int i = cnt - 1; i >= 0; --i)
  {
    position = ett_get_position(gEcs, entities[i]);

    if (!SDL_PointInRect(&(POINT){ position.x, position.y }, &gViewport))
    {
      ecs_destroy(gEcs, entities[i]);
    }
  }
}

static void destroy_tagged_entities()
{

  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Hand* equipment;

  ecs_raw(gEcs, DESTROYED_TAG, &entities, NULL, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    if ((equipment = ecs_get(gEcs, entities[i], HAND)))
    {
      if (equipment->weapon != ECS_NULL_ENT)
        ecs_destroy(gEcs, equipment->weapon);
    }
    ecs_destroy(gEcs, entities[i]);
  }
}

void late_destroying_system()
{
  destroy_offscreen_entities();
  destroy_tagged_entities();
}
