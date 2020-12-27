#include "components.h"
#include <stdlib.h>
#include <string.h>
#include <toolbox/sprite-sheet.h>

const EcsType g_comp_types[NUM_COMPONENTS] = {
  [VISUAL] =
      (EcsType){
          .size = sizeof(Visual),
      },
  [TRANSFORM] =
      (EcsType){
          .size = sizeof(Transform),
      },
  [ANIMATOR] =
      (EcsType){
          .size    = sizeof(Animator),
          .fini_fn = (ecs_comp_fini_fn_t)animator_fini,
      },
  [PLAYER_TAG] =
      (EcsType){
          .size = sizeof(PlayerTag),
      },
  [CONTROLLER] =
      (EcsType){
          .size = sizeof(Controller),
      },
  [EQUIPMENT] =
      (EcsType){
          .size = sizeof(Equipment),
      },
  [HEATH] =
      (EcsType){
          .size = sizeof(Heath),
      },
  [HITBOX] =
      (EcsType){
          .size    = sizeof(HitBox),
          .init_fn = (ecs_comp_init_fn_t)hitbox_init,
      },
  [ENEMY_TAG] =
      (EcsType){
          .size = sizeof(EnemyTag),
      },
  [HEAL_BAR] =
      (EcsType){
          .size = sizeof(HealthBar),
      },
  [LIFE_SPAN] =
      (EcsType){
          .size = sizeof(LifeSpan),
      },
  [MOTION] =
      (EcsType){
          .size = sizeof(Motion),
      },
  [PROJECTILE] =
      (EcsType){
          .size = sizeof(Projectile),
      },
  [TAG_TO_BE_DESTROYED] =
      (EcsType){
          .size = sizeof(TagToBeDestroyed),
      },
  [WEAPON_CORE] =
      (EcsType){
          .size = sizeof(WeaponCore),
      },
  [DAMAGE_OUTPUT] =
      (EcsType){
          .size = sizeof(DamageOutput),
      },
  [WEAPON_SKILL_SWING] =
      (EcsType){
          .size = sizeof(wpskl_Swing),
      },
  [WEAPON_SKILL_CHARGE] =
      (EcsType){
          .size = sizeof(wpskl_Charge),
      },
  [DROP] =
      (EcsType){
          .size = sizeof(Drop),
      },
  [INVULNERABLE] =
      (EcsType){
          .size = sizeof(Invulnerable),
      },
  [CAMERA_TARGET_TAG] =
      (EcsType){
          .size = sizeof(CameraTargetTag),
      },
  [AI_AGENT] =
      (EcsType){
          .size    = sizeof(AIAgent),
          .fini_fn = (ecs_comp_fini_fn_t)ai_agent_fini,
      },
  [BTV_MOVE_DESTINATION] =
      (EcsType){
          .size = sizeof(btv_MoveDestination),
      },
  [BTV_PATH] =
      (EcsType){
          .size = sizeof(btv_Path),
      },
  [BTV_FOLLOWING_TARGET] =
      (EcsType){
          .size = sizeof(btv_FollowingTarget),
      },
  [SPOT] =
      (EcsType){
          .size = sizeof(Spot),
      },
  [WEAPON_SKILL_THUNDER_STORM] =
      (EcsType){
          .size = sizeof(wpskl_ThunderStorm),
      },
};

Animation*
animation_init(Animation* anim, SDL_Texture* tex, u32 x, u32 y, u32 row, u32 col, u32 sw, u32 sh)
{
  sprite_sheet_init_1(&anim->sheet, tex, x, y, sw, sh, row, col);
  anim->frame_duration = 1;
  return anim;
}

void animation_fini(Animation* anim)
{
  sprite_sheet_fini(&anim->sheet);
}

Animator* animator_init(Animator* animator, const Animation* anims, u32 cnt)
{
  animator->anims        = malloc(cnt * sizeof(Animation));
  animator->cnt          = cnt;
  animator->elapsed      = 0;
  animator->current_anim = 0;
  memmove(animator->anims, anims, cnt * sizeof(Animation));
  return animator;
}

void animator_fini(Animator* animator)
{
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

void hitbox_init(HitBox* h)
{
  h->proxy_id = RTREE_NULL_NODE;
}
