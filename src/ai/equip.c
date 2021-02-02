#include "ai/equip.h"
#include "behaviour_tree.h"
#include "components.h"
#include "types.h"
#include "utils.h"

struct BTTask_Equip
{
  BT_EXTEND_NODE(BTNode)
  u16 weapon_id;
};

static void __vtbl_init(BTNodeVtbl* vtbl);

static BTTask_Equip* __init(BTTask_Equip* self, u16 weapon_id);

static void __finish(BTTask_Equip* self, Ecs* ecs, ecs_entity_t entity, BTStatus finish_status);

static void __abort(BTTask_Equip* self, Ecs* ecs, ecs_entity_t entity);

static BTStatus __exec(BTTask_Equip* self, Ecs* ecs, ecs_entity_t entity);

BT_STATIC_VTBL_INST_FN(BTNode, _)
BT_ALLOC_FN(BTTask_Equip, _)

static void __vtbl_init(BTNodeVtbl* vtbl)
{
  bt_node_vtbl_init(vtbl);
  vtbl->parent = bt_node_vtbl_inst();
  vtbl->exec   = (bt_exec_fn_t)__exec;
  vtbl->abort  = (bt_abort_fn_t)__abort;
  vtbl->finish = (bt_finish_fn_t)__finish;
}

BTTask_Equip* bt_task_equip_new(u16 weapon_id)
{
  return __init(__alloc(), weapon_id);
}

static BTTask_Equip* __init(BTTask_Equip* self, u16 weapon_id)
{
  bt_node_init((BTNode*)self);
  self->weapon_id = weapon_id;
  return self;
}

static void
__abort(SDL_UNUSED BTTask_Equip* self, SDL_UNUSED Ecs* ecs, SDL_UNUSED ecs_entity_t entity)
{
}

static void __finish(SDL_UNUSED BTTask_Equip* self,
                     SDL_UNUSED Ecs*         ecs,
                     SDL_UNUSED ecs_entity_t entity,
                     SDL_UNUSED BTStatus     finish_status)
{
}

static BTStatus __exec(BTTask_Equip* self, Ecs* ecs, ecs_entity_t entity)
{
  Equipment*  equipment   = ecs_get(ecs, entity, EQUIPMENT);
  AttackMask* attack_mask = ecs_get(ecs, entity, ATTACK_MASK);

  if (equipment == NULL)
    return BT_STATUS_FAILURE;
  if (equipment->weapon != ECS_NULL_ENT)
  {
    WeaponAttributes* weapon_attributes = ecs_get(ecs, equipment->weapon, WEAPON_ATTRIBUTES);
	if (weapon_attributes->type_id == self->weapon_id)
		return BT_STATUS_FAILURE;
  }

  if (self->weapon_id == WEAPON_ID_NULL)
    return BT_STATUS_FAILURE;

  ecs_entity_t weapon = g_weapon_create_fn_tbl[self->weapon_id](ecs, attack_mask->value);
  equip(ecs, entity, weapon);

  return BT_STATUS_SUCCESS;
}
