#include "camera_sys.h"
#include <components.h>

extern SDL_Rect g_viewport;
extern Ecs*     g_ecs;
void            camera_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Transform*    transform;

  Vec2 target;

  ecs_raw(g_ecs, CAMERA_TARGET_TAG, &entities, NULL, &cnt);

  if (cnt > 0 && (transform = ecs_get(g_ecs, entities[0], TRANSFORM)))
  {
    target.x = transform->position.x - g_viewport.w / 2.f;
    target.y = transform->position.y - g_viewport.h / 2.f;
    g_viewport.x += (target.x - g_viewport.x) * 0.2f;
    g_viewport.y += (target.y - g_viewport.y) * 0.2f;
  }
}
