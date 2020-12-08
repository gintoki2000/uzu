#include "components.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox/sprite-sheet.h>
Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh)
{
  sprite_sheet_init_1(&anim->sheet, tex, x, y, sw, sh, row, col);
  anim->frame_duration = 1;
  return anim;
}

void animation_fini(Animation* anim) { sprite_sheet_fini(&anim->sheet); }

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt)
{
  animator->anims = malloc(cnt * sizeof(Animation));
  animator->cnt   = cnt;
  memmove(animator->anims, anims, cnt * sizeof(Animation));
  return animator;
}

void animator_fini(Animator* animator)
{
  INFO("get called\n");
  for (u32 i = 0; i < animator->cnt; ++i)
  {
    animation_fini(&animator->anims[i]);
  }
  free(animator->anims);
}

void visual_set_anchor_to_center(Visual* v)
{
  v->anchor.x = v->sprite.rect.w / 2;
  v->anchor.y = v->sprite.rect.h / 2;
}

void ai_agent_fini(AIAgent* ai_agent)
{
  bt_node_del(ai_agent->root);
  ai_agent->root = NULL;
}
