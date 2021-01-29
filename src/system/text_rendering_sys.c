#include "text_rendering_sys.h"
#include "components.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

extern Ecs*          g_ecs;
extern SDL_Renderer* g_renderer;
extern SDL_Rect      g_viewport;

void text_rendering_system_update()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Text*      texts;
  Transform* transform;

  ecs_raw(g_ecs, TEXT, &entities, (void**)&texts, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      FC_DrawColor(texts[i].font,
                   g_renderer,
                   transform->position.x - g_viewport.x,
                   transform->position.y - g_viewport.y,
                   texts[i].color,
                   texts[i].value);
    }
  }
}
