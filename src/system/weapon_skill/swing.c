#include "swing.h"
#include "../event_messaging_sys.h"
#include <components.h>

#define SWINGING_SYSTEM_STEP 8
#define SWINGING_SYSTEM_INTERVAL 1
#define SWINGING_SYSTEM_DAMAGE_ADJ 1.0
extern Ecs* g_ecs;

typedef struct CBSwing_QueryHitEntitiesArgs
{
  ecs_entity_t dealer;
  int          damage;
  Vec2         force;
} CBSwing_QueryHitEntitiesArgs;

static BOOL __cb_query_hit_entities(CBSwing_QueryHitEntitiesArgs* args, ecs_entity_t e)
{
  ems_broadcast(MSG_DEAL_DAMAGE,
                &(MSG_DealDamage){ .dealer      = args->dealer,
                                   .receiver    = e,
                                   .damage      = args->damage,
                                   .impact      = TRUE,
                                   .impact_time = 10,
                                   .force       = args->force });
  return TRUE;
}

void swing_weapon_skl_system_update(void)
{
  ecs_entity_t* entities;
  ecs_size_t    cnt;

  wpskl_Swing* skl;
  WeaponAttributes*  base;
  Visual*      visual;
  Transform*   transform;
  Controller*  wearer_controller;

  ecs_raw(g_ecs, WEAPON_SKILL_SWING, &entities, (void**)&skl, &cnt);

  for (int i = 0; i < cnt; ++i)
  {
  }
}
