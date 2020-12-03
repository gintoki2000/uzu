#include "sword_system.h"
#include "collision_system.h"
#include "health_system.h"
#include <components.h>
#include <entity_factory.h>
#include <stdio.h>
#include <toolbox/toolbox.h>

#define INTERVAL 1
#define STEP 7

static void process(Ecs*            ecs,
                    ecs_entity_t    e,
                    GenericSword*   gswd,
                    WeaponAction* cmd,
                    Visual*         vs,
                    Transform*      tx)
{
  (void)ecs;

  switch (gswd->state)
  {
  case SWORD_STATE_IDLE:
  {
    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      gswd->state = SWORD_STATE_REGULAR_ATK;
      gswd->step = 1;
      gswd->timer = 0;
      tx->rot = 20.0 * FLIP_TO_SIGN(vs->flip);
      ecs_add(ecs, e, PLAYER_WEAPON_TAG);
    }
  }
  break;
  case SWORD_STATE_REGULAR_ATK:
  {
    gswd->timer++;
    if (gswd->timer == INTERVAL)
    {
      gswd->timer = 0;
      gswd->step++;
      if (gswd->step == STEP)
      {
        gswd->state = SWORD_STATE_IDLE;
        tx->rot = 0.0;
        ecs_rmv(ecs, e, PLAYER_WEAPON_TAG);
      }
      else
      {
        tx->rot = gswd->step * 20.0 * FLIP_TO_SIGN(vs->flip);
      }
    }
  }
  }
}

static void on_player_weapon_hit_enemy(Ecs* ecs, const WeaponHitEnemyEvent* evt)
{
  GenericSword* gswd;
  Transform*    enemy_tx;
  if ((gswd = ecs_get(ecs, evt->weapon, GENERIC_SWORD)) != NULL)
  {
    enemy_tx = ecs_get(ecs, evt->enemy, TRANSFORM);
    make_blood_stain_effect(ecs, enemy_tx->pos);
    health_system_apply_damage(ecs, evt->enemy, gswd->atk);
  }
}

void generic_sword_system_init(Ecs* ecs)
{
  collision_system_connect(SIG_PLAYER_WEAPON_COLLIED_W_ENEMY,
                           ecs,
                           (sig_handler_fn_t)on_player_weapon_hit_enemy);
}

void generic_sword_system_fini() {}

void GenericSwordSystem(Ecs* ecs)
{
  ecs_entity_t*   ett;
  WeaponAction* cmd;
  GenericSword*   gswd;
  Visual*         vs;
  Transform*      tx;
  ecs_size_t      cnt;

  ecs_data(ecs, GENERIC_SWORD, &ett, (void**)&gswd, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, ett[i], WEAPON_ACTION);
    vs = ecs_get(ecs, ett[i], VISUAL);
    tx = ecs_get(ecs, ett[i], TRANSFORM);
    process(ecs, ett[i], &gswd[i], cmd, vs, tx);
  }
}
