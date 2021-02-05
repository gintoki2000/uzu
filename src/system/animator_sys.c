#include "components.h"
#include "system/game_logic.h"
#include "ecs/ecs.h"

extern Ecs* g_ecs;

void animator_system_update(void)
{
  ecs_entity_t* ett;
  Animator*     animator;
  Visual*       visual;
  ecs_size_t    cnt;

  const Animation* curr;
  int              idx;
  ecs_raw(g_ecs, ANIMATOR, &ett, (void**)&animator, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    visual = ecs_get(g_ecs, ett[i], VISUAL);
    curr   = &animator[i].anims[animator[i].current_anim];

    idx            = animator[i].elapsed / curr->frame_duration;
    idx            = idx % curr->sheet.count;
    visual->sprite = curr->sheet.sprites[idx];
    animator[i].elapsed++;
  }
}
