#include "system/game_logic.h"
#include "components.h"
#include "ecs/ecs.h"
#include "toolbox/toolbox.h"

extern ecs_Registry*          gRegistry;
extern SDL_Renderer* gRenderer;
extern SDL_Rect      gViewport;

void text_rendering_system()
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  Text*      texts;
  Transform* transform;

  ecs_raw(gRegistry, TEXT, &entities, (void**)&texts, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    if ((transform = ecs_get(gRegistry, entities[i], TRANSFORM)))
    {
      FC_DrawEffect(texts[i].font,
                    gRenderer,
                    transform->position.x - gViewport.x,
                    transform->position.y - gViewport.y,
                    texts[i].effect,
                    texts[i].value);
    }
  }
}
