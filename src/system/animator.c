#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "system/game_logic.h"

extern Ecs* gEcs;

void animator_system(void)
{
  ecs_entity_t* entities;
  Animator*     animator;
  Visual*       visual;
  ecs_size_t    cnt;

  const Animation*         curr;
  int                      idx;
  RECT                     rect;

  ecs_raw(gEcs, ANIMATOR, &entities, (void**)&animator, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    visual = ecs_get(gEcs, entities[i], VISUAL);
    curr   = &animator[i].anims[animator[i].currentAnimation];

    idx = animator[i].elapsed / curr->frameDuration;
    idx = idx % curr->count;

    rect.x = curr->offsetX + curr->spriteWidth * idx;
    rect.y = curr->offsetY;
    rect.w = curr->spriteWidth;
    rect.h = curr->spriteHeight;

    visual->sprite.textureId = curr->textureId;
    visual->sprite.rect       = rect;

    animator[i].elapsed++;
  }
}
