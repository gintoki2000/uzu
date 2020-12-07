#include "camera_following_system.h"
#include <components.h>

extern SDL_Rect g_viewport;
void CameraFollowingSystem(Ecs *ecs)
{
  ecs_entity_t* entities;
  ecs_size_t cnt;
  Transform* transform;

  ecs_data(ecs, CAMERA_TARGET_TAG, &entities, NULL, &cnt);

  if (cnt > 0 && (transform = ecs_get(ecs, entities[0], TRANSFORM)))
  {
    g_viewport.x = transform->pos.x - g_viewport.w / 2;
    g_viewport.y = transform->pos.y - g_viewport.h / 2;
  }
}
