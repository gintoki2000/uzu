#include <components.h>
#include "camera_sys.h"

extern SDL_Rect g_viewport;
void sys_camera_update(Ecs *ecs)
{
  ecs_entity_t* entities;
  ecs_size_t cnt;
  Transform* transform;

  Vec2 target;


  ecs_data(ecs, CAMERA_TARGET_TAG, &entities, NULL, &cnt);

  if (cnt > 0 && (transform = ecs_get(ecs, entities[0], TRANSFORM)))
  {
    target.x = transform->pos.x - g_viewport.w / 2; 
    target.y = transform->pos.y - g_viewport.h / 2;
    g_viewport.x += (target.x - g_viewport.x) * 0.1f;
    g_viewport.y += (target.y - g_viewport.y) * 0.1f;
  }
}
