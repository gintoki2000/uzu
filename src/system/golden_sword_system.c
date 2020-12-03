#include "golden_sword_system.h"
#include "collision_system.h"
#include "health_system.h"
#include <components.h>
#include <entity_factory.h>

#define INTERVAL 1
#define STEP 7

enum
{
  GOLDEN_SWORD_STATE_IDLE,
  GOLDEN_SWORD_STATE_REGULAR_ATK,
  GOLDEN_SWORD_STATE_THUNDER_STK,
};

static void on_weapon_hit_enemy(Ecs* ecs, const WeaponHitEnemyEvent* event);
static void process(Ecs*          ecs,
                    ecs_size_t    entity,
                    WeaponAction* weapon_action,
                    Transform*    transfom,
                    Visual*       visual,
                    GoldenSword*  golden_sword);

void golden_sword_system_init(Ecs* ecs)
{
  collision_system_connect(SIG_PLAYER_WEAPON_COLLIED_W_ENEMY,
                           ecs,
                           (sig_handler_fn_t)on_weapon_hit_enemy);
}

void GoldenSwordSystem(Ecs* ecs)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;
  GoldenSword*  golden_swords;
  WeaponAction* weapon_action;
  Visual*       visual;
  Transform*    transfom;

  ecs_data(ecs, GOLDEN_SWORD, &entities, (void**)&golden_swords, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    weapon_action = ecs_get(ecs, entities[i], WEAPON_ACTION);
    visual = ecs_get(ecs, entities[i], VISUAL);
    transfom = ecs_get(ecs, TRANSFORM, TRANSFORM);
    process(ecs, entities[i], weapon_action, transfom, visual, &golden_swords[i]);
  }
}

static void on_weapon_hit_enemy(Ecs* ecs, const WeaponHitEnemyEvent* event)
{
  GoldenSword* golden_sword;
  if ((golden_sword = ecs_get(ecs, event->weapon, GOLDEN_SWORD)) != NULL)
  {
    make_golden_cross_hit_effect(ecs, ((Transform*)ecs_get(ecs, event->enemy, TRANSFORM))->pos);
    health_system_apply_damage(ecs, event->enemy, 3);
  }
}

static void process(Ecs*          ecs,
                    ecs_size_t    entity,
                    WeaponAction* weapon_action,
                    Transform*    transform,
                    Visual*       visual,
                    GoldenSword*  golden_sword)
{

  switch (golden_sword->state)
  {
  case GOLDEN_SWORD_STATE_IDLE:
  {
    if (weapon_action->action == WEAPON_ACTION_REGULAR_ATK)
    {
      weapon_action->action = WEAPON_ACTION_NONE;
      golden_sword->state = GOLDEN_SWORD_STATE_REGULAR_ATK;
      golden_sword->step = 1;
      golden_sword->timer = 0;
      transform->rot = 20.0 * FLIP_TO_SIGN(visual->flip);
      ecs_add(ecs, entity, PLAYER_WEAPON_TAG);
    }
  }
  break;
  case GOLDEN_SWORD_STATE_REGULAR_ATK:
  {
    golden_sword->timer++;
    if (golden_sword->timer == INTERVAL)
    {
      golden_sword->timer = 0;
      golden_sword->step++;
      if (golden_sword->step == STEP)
      {
        golden_sword->state = GOLDEN_SWORD_STATE_IDLE;
        transform->rot = 0.0;
        ecs_rmv(ecs, entity, PLAYER_WEAPON_TAG);
      }
      else
      {
        transform->rot = golden_sword->step * 20.0 * FLIP_TO_SIGN(visual->flip);
      }
    }
  }
  }
}
