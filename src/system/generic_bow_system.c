#include "generic_bow_system.h"
#include "collision_system.h"
#include "health_system.h"
#include <components.h>
#include <entity_factory.h>

static void on_arrow_hit_enemy(Ecs* ecs, const WeaponHitEnemyEvent* evt);

void generic_bow_system_init(Ecs* ecs)
{
  collision_system_connect(SIG_PLAYER_WEAPON_COLLIED_W_ENEMY,
                           ecs,
                           (sig_handler_fn_t)on_arrow_hit_enemy);
}

void GenericBowSystem(Ecs* ecs)
{
  ecs_entity_t* bows;
  ecs_size_t    cnt;

  GenericBow*     g_bows;
  WeaponAction* cmd;
  Transform*      transform;
  Visual*         visual;

  ecs_data(ecs, GENERIC_BOW, &bows, (void**)&g_bows, &cnt);
  for (int i = 0; i < cnt; ++i)
  {
    cmd = ecs_get(ecs, bows[i], WEAPON_ACTION);

    if (cmd->action == WEAPON_ACTION_REGULAR_ATK)
    {
      cmd->action = WEAPON_ACTION_NONE;
      transform = ecs_get(ecs, bows[i], TRANSFORM);
      visual = ecs_get(ecs, bows[i], VISUAL);
      make_arrow(ecs, transform->pos, VEC2(FLIP_TO_SIGN(visual->flip) * 100.f, 0.f));
    }
  }
}

static void on_arrow_hit_enemy(Ecs* ecs, const WeaponHitEnemyEvent* evt)
{
  Projectile* proj;
  if ((proj = ecs_get(ecs, evt->weapon, PROJECTILE)) != NULL)
  {
    health_system_apply_damage(ecs, evt->enemy, 2);
    ecs_add(ecs, evt->weapon, TAG_TO_BE_DESTROYED);
  }
}
