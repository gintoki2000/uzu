#include "components.h"
#include "ecs/ecs.h"
#include "resources.h"
#include "toolbox/sprite.h"
#include <entity_factory.h>

ecs_entity_t ettfactory_make_anime_sword(Ecs* ecs)
{
  SDL_Texture* texture;
  ecs_entity_t sword;

  /*component*/
  Visual*         visual;
  Transform*      transform;
  GenericSword*   generic_sword;
  WeaponCmdInput* cmd_input;

  texture = get_texture(TEX_ANIME_SWORD);

  sword = ecs_create(ecs);

  transform = ecs_add(ecs, sword, TRANSFORM);

  visual = ecs_add(ecs, sword, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;
  visual->rot = 0.0;

  generic_sword = ecs_add(ecs, sword, GENERIC_SWORD);
  generic_sword->atk = 1;
  generic_sword->range = 15;
  generic_sword->step = 0;
  generic_sword->timer = 0;

  cmd_input = ecs_add(ecs, sword, WP_CMD_INPUT);
  cmd_input->action = WEAPON_ACTION_NONE;

  return sword;
}

ecs_entity_t ettfactory_make_knight(Ecs* ecs, ecs_entity_t weapon)
{
  ecs_entity_t knight;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform* transform;
  Visual*    visual;
  Equipment* equipment;
  CmdInput*  cmd_input;
  Animator*  animator;
  HitBox*    hitbox;

  texture = get_texture(TEX_KNIGHT);
  animation_init(&anims[ANIM_STATE_HIT], texture, 0, 0, 1, 1, 16, 28);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 16 * 1, 0, 1, 4, 16, 28);
  animation_init(&anims[ANIM_STATE_RUN], texture, 16 * 5, 0, 1, 4, 16, 28);

  anims[ANIM_STATE_IDLE].frame_duration = 3;
  anims[ANIM_STATE_RUN].frame_duration = 2;

  knight = ecs_create(ecs);

  transform = ecs_add(ecs, knight, TRANSFORM);

  visual = ecs_add(ecs, knight, VISUAL);

  visual->anchor.x = 16 / 2;
  visual->anchor.y = 28 / 2;

  equipment = ecs_add(ecs, knight, EQUIPMENT);
  equipment->rhand = weapon;

  cmd_input = ecs_add(ecs, knight, COMMAND_INPUT);

  animator = ecs_add(ecs, knight, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed = 0;

  hitbox = ecs_add(ecs, knight, HITBOX);

  hitbox->hwidth = 16 / 2;
  hitbox->hheight = 28 / 2;
  hitbox->proxy_id = -1;
  return knight;
}

ecs_entity_t ettfactory_make_huge_demon(Ecs* ecs, ecs_entity_t weapon)
{
  ecs_entity_t demon;
  SDL_Texture* texture;
  Animation    anims[NUM_ANIM_STATES];

  /*components */
  Transform* transform;
  Visual*    visual;
  Equipment* equipment;
  CmdInput*  cmd_input;
  Animator*  animator;
  HitBox*    hitbox;

  texture = get_texture(TEX_BIG_DEMON);
  animation_init(&anims[ANIM_STATE_HIT], texture, 32 * 6, 0, 1, 1, 32, 36);
  animation_init(&anims[ANIM_STATE_IDLE], texture, 0, 0, 1, 4, 32, 36);
  animation_init(&anims[ANIM_STATE_RUN], texture, 32 * 4, 0, 1, 4, 32, 36);

  anims[ANIM_STATE_IDLE].frame_duration = 3;
  anims[ANIM_STATE_RUN].frame_duration = 2;

  demon = ecs_create(ecs);

  transform = ecs_add(ecs, demon, TRANSFORM);

  visual = ecs_add(ecs, demon, VISUAL);

  visual->anchor.x = 32 / 2;
  visual->anchor.y = 36 / 2;

  equipment = ecs_add(ecs, demon, EQUIPMENT);
  equipment->rhand = weapon;

  cmd_input = ecs_add(ecs, demon, COMMAND_INPUT);
  cmd_input->action = WEAPON_ACTION_NONE;

  animator = ecs_add(ecs, demon, ANIMATOR);
  animator_init(animator, anims, NUM_ANIM_STATES);
  animator->current_anim = ANIM_STATE_IDLE;
  animator->elapsed = 0;

  hitbox = ecs_add(ecs, demon, HITBOX);

  hitbox->hwidth = 32 / 2;
  hitbox->hheight = 36 / 2;
  hitbox->proxy_id = -1;

  return demon;
}

ecs_entity_t ettfactory_make_axe(Ecs* ecs)
{
  SDL_Texture* texture;
  ecs_entity_t axe;

  /*component*/
  Visual*         visual;
  Transform*      transform;
  GenericAxe*     generic_axe;
  WeaponCmdInput* cmd_input;

  texture = get_texture(TEX_AXE);

  axe = ecs_create(ecs);

  transform = ecs_add(ecs, axe, TRANSFORM);

  visual = ecs_add(ecs, axe, VISUAL);
  sprite_init(&visual->sprite, texture);
  visual->anchor.x = visual->sprite.rect.w / 2;
  visual->anchor.y = visual->sprite.rect.h;
  visual->rot = 0.0;

  generic_axe = ecs_add(ecs, axe, GENERIC_AXE);
  generic_axe->atk = 1;
  generic_axe->range = 15;
  generic_axe->step = 0;
  generic_axe->timer = 0;

  cmd_input = ecs_add(ecs, axe, WP_CMD_INPUT);
  cmd_input->action = WEAPON_ACTION_NONE;

  return axe;
}
