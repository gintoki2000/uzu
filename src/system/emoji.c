#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "sprite_renderer.h"

extern Ecs* gEcs;
extern RECT gViewport;

static Sprite _sprite_tbl[NUM_EMOJIES] = {
  { TEX_EMOJI_QUESTION, { 0, 0, 20, 19 } },
  { TEX_EMOJI_BANG, { 0, 0, 20, 19 } },
  { TEX_EMOJI_SILENT, { 0, 0, 20, 19 } },
  { TEX_EMOJI_HEART, { 0, 0, 20, 19 } },
};

void emoji_system(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  Emoji*        emoji;
  HitBox*       hitbox;
  Transform*    transform;
  RECT          dstRect;
  ecs_raw(gEcs, EMOJI, &entities, (void**)&emoji, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    ASSERT(emoji[i].id >= 0 && emoji[i].id < NUM_EMOJIES && "invalid emoji id");
    if (emoji[i].duration != -1 && (emoji[i].duration--) == 0)
    {
      INVOKE_EVENT(emoji[i].cbTimeOut, NULL);
      ecs_rmv(gEcs, entities[i], EMOJI);
      continue;
    }
    hitbox = ecs_get(gEcs, entities[i], HITBOX);
    if ((transform = ecs_get(gEcs, entities[i], TRANSFORM)))
    {
      dstRect.x = transform->position.x;
      dstRect.y = transform->position.y;
      dstRect.w = _sprite_tbl[emoji[i].id].rect.w;
      dstRect.h = _sprite_tbl[emoji[i].id].rect.h;

      dstRect.y += hitbox ? (-hitbox->anchor.y) : 0;
      dstRect.x += hitbox ? (hitbox->size.x / 2.f - hitbox->anchor.x) : 0;

      dstRect.x -= 13;
      dstRect.y -= 20;
      if (SDL_HasIntersection(&gViewport, &dstRect))
      {
        dstRect.x -= gViewport.x;
        dstRect.y -= gViewport.y;
        sprite_renderer_draw(_sprite_tbl[emoji[i].id], (POINT){ dstRect.x, dstRect.y }, 999);
      }
    }
  }
}
