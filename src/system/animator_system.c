#include <components.h>
#include <system/animator_system.h>

static char aniid_to_name[3][5] = {
  [ANIM_STATE_IDLE] = "IDLE",
  [ANIM_STATE_RUN]  = "RUN",
  [ANIM_STATE_HIT]  = "HURT",
};

static void process(Animator* animator, Visual* visual);

void AnimatorSystem(Ecs* ecs)
{
  ecs_entity_t* ett;
  Animator*     animator;
  Visual*       visual;
  ecs_size_t    cnt;

  const Animation* curr;
  int              idx;
  ecs_data(ecs, ANIMATOR, &ett, (void**)&animator, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    visual = ecs_get(ecs, ett[i], VISUAL);
    curr   = &animator[i].anims[animator[i].current_anim];

    idx            = animator[i].elapsed / curr->frame_duration;
    idx            = idx % curr->sheet.count;
    visual->sprite = curr->sheet.sprites[idx];
    animator[i].elapsed++;
  }
}
