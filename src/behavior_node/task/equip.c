#include "behaviour_tree.h"
#include "bttask.h"
#include "components.h"
#include "entity_utils.h"
#include "global.h"

struct BTTask_Equip
{
  BT_EXTEND_NODE(BTNode)
  u16 weapon_id;
};

#define TASK BTTask_Equip

static void     __vtbl_init(BTNodeVtbl* vtbl);
static TASK*    __init(TASK* self, u16 weapon_id);
static BTStatus __exec(TASK* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(TASK, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
}

TASK* bt_task_equip_new(u16 weapon_id)
{
  return __init(__alloc(), weapon_id);
}

static TASK* __init(TASK* self, u16 weapon_id)
{
  bt_node_init((BTNode*)self);
  self->weapon_id = weapon_id;
  return self;
}

static BTStatus __exec(TASK* self, Ecs* ecs, ecs_entity_t entity)
{
  Hand*       hand        = ecs_get(ecs, entity, HAND);
  AttackMask* attack_mask = ecs_get(ecs, entity, ATTACK_MASK);

  if (hand == NULL)
    return BT_STATUS_FAILURE;
  if (hand->weapon != ECS_NULL_ENT)
  {
    WeaponAttributes* weapon_attributes = ecs_get(ecs, hand->weapon, WEAPON_ATTRIBUTES);
    if (weapon_attributes->type_id == self->weapon_id)
      return BT_STATUS_FAILURE;
  }

  if (self->weapon_id == WEAPON_ID_NULL)
    return BT_STATUS_FAILURE;

  ecs_entity_t weapon = g_make_weapon_fn_tbl[self->weapon_id](ecs, attack_mask->value);
  equip(ecs, entity, weapon);

  return BT_STATUS_SUCCESS;
}
