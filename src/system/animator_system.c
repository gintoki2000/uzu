#include <components.h>
#include <system/animator_system.h>

static void process(Animator* animator, Visual* visual);

void AnimatorSystem(Ecs* ecs)
{
  ecs_entity_t* ett;
  Animator*     animator;
  Visual*       visual;
  ecs_size_t    cnt;

  ecs_data(ecs, ANIMATOR, &ett, (void**)&animator, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    visual = ecs_get(ecs, ett[i], VISUAL);
    process(&animator[i], visual);
  }
}
static void process(Animator* animator, Visual* visual)
{
  const Animation* curr;
  int idx;

  curr = &animator->anims[animator->current_anim];

  idx = animator->elapsed / curr->frame_duration;
  idx = idx % curr->sheet.count;

  visual->sprite = curr->sheet.sprites[idx];

  animator->elapsed++;
}
