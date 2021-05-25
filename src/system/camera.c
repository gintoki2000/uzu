#include "system/game_logic.h"
#include "components.h"

extern SDL_Rect gViewport;
extern Ecs*     gEcs;
void            camera_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Transform*    transform;

  Vec2 target;

  ecs_raw(gEcs, CAMERA_TARGET_TAG, &entities, NULL, &cnt);

  if (cnt > 0 && (transform = ecs_get(gEcs, entities[0], TRANSFORM)))
  {
    target.x = transform->position.x - gViewport.w / 2.f;
    target.y = transform->position.y - gViewport.h / 2.f;
    gViewport.x += (target.x - gViewport.x) * 0.2f;
    gViewport.y += (target.y - gViewport.y) * 0.2f;
  }
}
