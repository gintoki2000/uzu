#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "sprite_renderer.h"

extern Ecs* g_ecs;
extern RECT g_viewport;

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
  RECT          dst_rect;
  ecs_raw(g_ecs, EMOJI, &entities, (void**)&emoji, &cnt);
  for (int i = cnt - 1; i >= 0; --i)
  {
    ASSERT(emoji[i].id >= 0 && emoji[i].id < NUM_EMOJIES && "invalid emoji id");
    if (emoji[i].duration != -1 && (emoji[i].duration--) == 0)
    {
      INVOKE_EVENT(emoji[i].cb_time_out, NULL);
      ecs_rmv(g_ecs, entities[i], EMOJI);
      continue;
    }
    hitbox = ecs_get(g_ecs, entities[i], HITBOX);
    if ((transform = ecs_get(g_ecs, entities[i], TRANSFORM)))
    {
      dst_rect.x = transform->position.x;
      dst_rect.y = transform->position.y;
      dst_rect.w = _sprite_tbl[emoji[i].id].rect.w;
      dst_rect.h = _sprite_tbl[emoji[i].id].rect.h;

      dst_rect.y += hitbox ? (-hitbox->anchor.y) : 0;
      dst_rect.x += hitbox ? (hitbox->size.x / 2.f - hitbox->anchor.x) : 0;

      dst_rect.x -= 13;
      dst_rect.y -= 20;
      if (SDL_HasIntersection(&g_viewport, &dst_rect))
      {
        dst_rect.x -= g_viewport.x;
        dst_rect.y -= g_viewport.y;
        sprite_renderer_draw(_sprite_tbl[emoji[i].id], (POINT){ dst_rect.x, dst_rect.y }, 999);
      }
    }
  }
}
